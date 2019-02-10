#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "grafika.h"

struct fb_var_screeninfo screen_info;
struct fb_fix_screeninfo fixed_info;



char *buffer = NULL;
long pixels[10000];
int pixel_count = 0;

void write_black_pixel(char *buffer, long offset)
{
    buffer[offset] = 0x00;
    buffer[offset + 1] = 0x00;
    buffer[offset + 2] = 0x00;
    buffer[offset + 3] = 0x00;
}

void write_white_pixel(char *buffer, long offset)
{
    buffer[offset] = 0xFF;
    buffer[offset + 1] = 0xFF;
    buffer[offset + 2] = 0xFF;
    buffer[offset + 3] = 0xFF;
}

void write_green_pixel(char *buffer, long offset)
{
    buffer[offset] = 0x00;
    buffer[offset + 1] = 0xFF;
    buffer[offset + 2] = 0x00;
    buffer[offset + 3] = 0xFF;
}

void write_red_pixel(char *buffer, long offset)
{
    buffer[offset] = 0xFF;
    buffer[offset + 1] = 0x00;
    buffer[offset + 2] = 0x00;
    buffer[offset + 3] = 0xFF;
}

void write_blue_pixel(char *buffer, long offset)
{
    buffer[offset] = 0x00;
    buffer[offset + 1] = 0x00;
    buffer[offset + 2] = 0xFF;
    buffer[offset + 3] = 0xFF;
}

void move_pixels()
{
    for (int i = 0; i < pixel_count; i++)
    {
        write_black_pixel(buffer, pixels[i]);
        pixels[i] = pixels[i] + 1;
        if (i < 2500)
        {
            write_white_pixel(buffer, pixels[i]);
        }
        if (i < 5000)
        {
            write_red_pixel(buffer, pixels[i]);
        }
        if (i < 7500)
        {
            write_green_pixel(buffer, pixels[i]);
        }
        if (i < 10000)
        {
            write_blue_pixel(buffer, pixels[i]);
        }
    }
    sleep(0.7);
}

void print_pixel(long x, long y)
{
    long offset = x * screen_info.bits_per_pixel / 8 + y * fixed_info.line_length;
    write_white_pixel(buffer, offset);
    pixels[pixel_count] = offset;
    pixel_count++;
}

void print_line( Point p1,  Point p2)
{
    int dx = fabs(p2.x - p1.x);
    int dy = fabs(p2.y - p1.y);
    int signx = p2.x > p1.x ? 1 : -1;
    int signy = p2.y > p1.y ? 1 : -1;
    int x = p1.x;
    int y = p1.y;
    int temp;
    int interchange = 0;
    int error;
    if (dy > dx)
    {
        temp = dx;
        dx = dy;
        dy = temp;
        interchange = 1;
    }

    error = (2 * dy) - dx;
    for (int i = 0; i < dx; i++)
    {
        if (error < 0)
        {
            if (interchange == 1)
            {
                y = y + signy;
            }
            else
            {
                x = x + signx;
            }
            error = error + (2 * dy);
        }
        else
        {
            y = y + signy;
            x = x + signx;
            error = error + 2 * (dy - dx);
        }
        print_pixel(x, y);
    }
}

void write_point( Point p)
{
    printf("point at :%d,%d\n", p.x, p.y);
}

void draw_all_quadrant( Point center, int x, int y)
{
    print_pixel(center.x + x, center.y + y);
    print_pixel(center.x + x, center.y - y);
    print_pixel(center.x - x, center.y + y);
    print_pixel(center.x - x, center.y - y);
    print_pixel(center.x + y, center.y + x);
    print_pixel(center.x - y, center.y + x);
    print_pixel(center.x + y, center.y - x);
    print_pixel(center.x - y, center.y - x);
}

// pake ini buat gambar lingkaran
void draw_circle( Point center, int r)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    draw_all_quadrant(center, x, y);
    while (x <= y)
    {
        x++;
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        draw_all_quadrant(center, x, y);
    }
}

// pake ini buat gambar polygon
void draw_polygon(polygon poly) {
    int i;
    for (i = 0; i < poly.N-1; i++) {
        print_line(poly.points[i], poly.points[i+1]);
    }
    print_line(poly.points[poly.N-1],poly.points[0]);
}

// pake ini buat gambar persegi
void draw_rect( Point p1,  Point p2) {
     Point p3, p4;
    p3.x = p1.x;
    p3.y = p2.y;
    p4.x = p2.x;
    p4.y = p1.y;
    print_line(p2, p3);
    print_line(p3, p1);
    print_line(p1, p4);
    print_line(p4, p2);
}

rect* read_file_sqr(char* filename) {
	FILE *file = fopen(filename, "r");
    rect* Re = malloc(sizeof(rect)*20);
    int i = 0;

	while(fscanf(file, "%d,%d %d,%d", &Re[i].p1.x,&Re[i].p1.y,&Re[i].p2.x,&Re[i].p2.y) != EOF){
        i++;
	}
	fclose(file);
    return Re;
}

polygon* read_file_polygon(char* filename) {
  FILE *file = fopen(filename, "r");
  polygon* Pol = malloc(sizeof(polygon)*20);
  int i=0;
  int ip;

  while(fscanf(file, "%d", &Pol[i].N) != EOF){
    Pol[i].points = malloc(sizeof(Point)*Pol[i].N);
    for (ip=0; ip<Pol[i].N; ip++) {
      fscanf(file, "%d,%d", &Pol[i].points[ip].x, &Pol[i].points[ip].y);
    }
    i++;
  }
  fclose(file);
  return Pol;
}

void read_file_crc(char* filename) {
	FILE *file = fopen(filename, "r");
	 Point p;
	int radius;

	while(fscanf(file, "%d,%d %d", &p.x,&p.y,&radius) != EOF){
        draw_circle(p, radius);
	}
	fclose(file);
}

void clear_screen(char *buffer){
    for (long i = 0; i < 4147480; i++)
    {
        write_black_pixel(buffer, i);
    }
}

void translate(int dx, int dy){

}

int main()
{
    char a;
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
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          fd,
                          0);
            if (buffer != MAP_FAILED)
            {
                /*    buffer + x * screen_info.bits_per_pixel/8
			 *           + y * fixed_info.line_length
			 */
                r = 0;
                // for (long i = 0; i < 4147480; i++)
                // {
                //     write_black_pixel(buffer, i);
                // }
                clear_screen(buffer);


                //  Point p;
                // p.x = 100;
                // p.y = 250;
                // int r = 50;
                // draw_circle(p, r);
                //  Point p2;
                // p2.x = 200;
                // p2.y = 300;
                // draw_rect(p, p2);
                // rect* arr_rect = malloc(sizeof(rect)*50);
                // arr_rect = read_file_sqr("sqr.txt");
                // draw_rect(arr_rect[0].p1,arr_rect[0].p2);

                polygon* arr_polygon = malloc(sizeof(polygon)*20);
                arr_polygon = read_file_polygon("polygon.txt");
                draw_polygon(arr_polygon[0]);

                scanf("%c",&a);

                // arr_rect[0].p2.x = arr_rect[0].p2.x+300;
                // arr_rect[0].p1.x = arr_rect[0].p1.x+300;

                // clear_screen(buffer);

                // draw_rect(arr_rect[0].p1,arr_rect[0].p2);
            }
        }
    }
    if (buffer && buffer != MAP_FAILED)
        munmap(buffer, buflen);
    if (fd >= 0)
        close(fd);

    scanf("%c",&a);

    return r;
}
