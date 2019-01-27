#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <math.h>

struct fb_var_screeninfo screen_info;
struct fb_fix_screeninfo fixed_info;
struct Point
{
  int x;
  int y;
};

char *buffer = NULL;
long pixels[10000];
int pixel_count = 0;

void write_black_pixel(char * buffer, long offset) {
	buffer[offset] = 0x00;
	buffer[offset+1] = 0x00;
	buffer[offset+2] = 0x00;
	buffer[offset+3] = 0x00;
}

void write_white_pixel(char * buffer, long offset) {
	buffer[offset] = 0xFF;
	buffer[offset+1] = 0xFF;
	buffer[offset+2] = 0xFF;
	buffer[offset+3] = 0xFF;
}

void write_green_pixel(char * buffer, long offset) {
	buffer[offset] = 0x00;
	buffer[offset+1] = 0xFF;
	buffer[offset+2] = 0x00;
	buffer[offset+3] = 0xFF;
}

void write_red_pixel(char * buffer, long offset) {
	buffer[offset] = 0xFF;
	buffer[offset+1] = 0x00;
	buffer[offset+2] = 0x00;
	buffer[offset+3] = 0xFF;
}

void write_blue_pixel(char * buffer, long offset) {
	buffer[offset] = 0x00;
	buffer[offset+1] = 0x00;
	buffer[offset+2] = 0xFF;
	buffer[offset+3] = 0xFF;
}

void move_pixels() {
    for (int i = 0; i < pixel_count; i++) {
        write_black_pixel(buffer, pixels[i]);
        pixels[i] = pixels[i] + 1;
        if (i < 2500) {
            write_white_pixel(buffer, pixels[i]);
        }
        if (i < 5000) {
            write_red_pixel(buffer, pixels[i]);
        }
        if (i < 7500) {
            write_green_pixel(buffer, pixels[i]);
        }
        if (i < 10000) {
           write_blue_pixel(buffer, pixels[i]);
        }
    }
    sleep(0.7);

}

void print_pixel(long x, long y) {
	long offset = x * screen_info.bits_per_pixel/8 + y * fixed_info.line_length;
	write_white_pixel(buffer, offset);
    pixels[pixel_count] = offset;
    pixel_count++;
}

void print_line(struct Point p1,struct Point p2) {
  int dx = fabs(p2.x-p1.x);
  int dy = fabs(p2.y-p1.y);
  int control = 2*dy - dx;
  int y = p1.y;
  for(int i = p1.x; i < p2.x; i++){
    print_pixel(i,y);
    if(control >= 0) {
      y++;
      control -= 2*dx;
    }
    control += 2*dy;
  }
}

int main()
{
   size_t buflen;
   int fd = -1;
   int r = 1;

   fd = open("/dev/fb0", O_RDWR);
   if (fd >= 0)
   {
      if (!ioctl(fd, FBIOGET_VSCREENINFO, &screen_info) &&
          !ioctl(fd, FBIOGET_FSCREENINFO, &fixed_info))
	  {
         buflen = screen_info.yres_virtual * fixed_info.line_length;
         buffer = mmap(NULL,
                       buflen,
                       PROT_READ|PROT_WRITE,
                       MAP_SHARED,
                       fd,
                       0);
         if (buffer != MAP_FAILED)
	 {
			/*    buffer + x * screen_info.bits_per_pixel/8
			 *           + y * fixed_info.line_length
			 */
		r = 0;
		for (long i = 0; i < 1000000; i++) {
			write_black_pixel(buffer,i);	
		}
        FILE* file = fopen("result.txt", "r");
        char line[25];

        while (fgets(line, sizeof(line), file)) {
            int i = 0;
            int j = 0;
            long temp[2];
            temp[0] = 0;
            temp[1] = 0;
            while (line[i] != '\n') {
                if (line[i] == ' ') {
                    j++;
                } else {
                    temp[j] = temp[j] * 10 + (line[i] - '0');
                }
                i++;
            }
            print_pixel(temp[0], temp[1]);
        }

        fclose(file);

        //example for print line
        struct Point p1;
        struct Point p2;
        p1.x = 82;
        p1.y = 216;
        p2.x = 176;
        p2.y = 128;
        print_line(p1,p2);

        for (int i = 0; i < 10000; i++) {
            move_pixels();
        }
	 }
   	}
   }
   if (buffer && buffer != MAP_FAILED)
      munmap(buffer, buflen);
   if (fd >= 0)
      close(fd);

   return r;
}

