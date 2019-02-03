#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
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

void print_line(struct Point p1, struct Point p2)
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

void write_point(struct Point p)
{
    printf("point at :%d,%d\n", p.x, p.y);
}

void draw_all_quadrant(struct Point center, int x, int y) 
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
void draw_circle(struct Point center, int r)
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
void draw_polygon(struct Point points[], int total_point) {
    int i;
    for (i = 0; i < total_point-1; i++) {
        print_line(points[i], points[i+1]);
    }
    print_line(points[total_point-1],points[0]);
}

// pake ini buat gambar persegi
void draw_rect(struct Point p1, struct Point p2) {
    struct Point p3, p4;
    p3.x = p1.x;
    p3.y = p2.y;
    p4.x = p2.x;
    p4.y = p1.y;
    print_line(p2, p3);
    print_line(p3, p1);
    print_line(p1, p4);
    print_line(p4, p2);
}

void read_file_sqr(char* filename) {
	FILE *file = fopen(filename, "r");
	struct Point p1;
    struct Point p2;

	while(fscanf(file, "%d,%d %d,%d", &p1.x,&p1.y,&p2.x,&p2.y) != EOF){
        draw_rect(p1, p2);
	}
	fclose(file);
}

void read_file_crc(char* filename) {
	FILE *file = fopen(filename, "r");
	struct Point p;
	int radius;

	while(fscanf(file, "%d,%d %d", &p.x,&p.y,&radius) != EOF){
        draw_circle(p, radius);
	}
	fclose(file);
}

void read_file_pol_three(char* filename) {
    FILE *file = fopen(filename, "r");
    
    struct Point p[4];
    struct Point p0;
    struct Point p1;
    struct Point p2;
    //Initialize
    // for (int i = 0; i<3; i++) {
    //     p[i].x = -1;
    //     p[i].y = -1;
    // }
    
    while(fscanf(file, "%d,%d %d,%d %d,%d", &p0.x,&p0.y,&p1.x,&p1.y,&p2.x,&p2.y) != EOF){
        printf("hehe\n");
        // p[0] = p0;
        // p[1] = p1;
        // p[2] = p2;
        // draw_polygon(p, 3);

        // for (int i = 0; i<3; i++) {
        //     p[i].x = -1;
        //     p[i].y = -1;
        // }
    }
    fclose(file);
}

void read_file_pol_four(char* filename) {
    FILE *file = fopen(filename, "r");
    
    struct Point p[5];
    
    //Initialize
    for (int i = 0; i<4; i = i+1) {
        p[i].x = -1;
        p[i].y = -1;
    }
    
    while(fscanf(file, "%d,%d %d,%d %d,%d %d,%d", &p[0].x,&p[0].y,&p[1].x,&p[1].y,&p[2].x,&p[2].y,&p[3].x,&p[3].y) != EOF){
        
        draw_polygon(p, 4);
        
        for (int i = 0; i<4; i = i+1) {
            p[i].x = -1;
            p[i].y = -1;
        }
    }
    fclose(file);
}

void read_file_pol_five(char* filename) {
    FILE *file = fopen(filename, "r");
    
    struct Point p[6];
    
    //Initialize
    for (int i = 0; i<5; i = i+1) {
        p[i].x = -1;
        p[i].y = -1;
    }
    
    while(fscanf(file, "%d,%d %d,%d %d,%d %d,%d %d,%d", &p[0].x,&p[0].y,&p[1].x,&p[1].y,&p[2].x,&p[2].y,&p[3].x,&p[3].y,&p[4].x,&p[4].y) != EOF){
        
        draw_polygon(p, 5);
        
        for (int i = 0; i<5; i = i+1) {
            p[i].x = -1;
            p[i].y = -1;
        }
    }
    fclose(file);
}

void read_file_pol_six(char* filename) {
    FILE *file = fopen(filename, "r");
    
    struct Point p[7];
    
    //Initialize
    for (int i = 0; i<6; i = i+1) {
        p[i].x = -1;
        p[i].y = -1;
    }
    
    while(fscanf(file, "%d,%d %d,%d %d,%d %d,%d %d,%d %d,%d", &p[0].x,&p[0].y,&p[1].x,&p[1].y,&p[2].x,&p[2].y,&p[3].x,&p[3].y,&p[4].x,&p[4].y,&p[5].x,&p[5].y) != EOF){
        
        draw_polygon(p, 6);
        
        for (int i = 0; i<6; i = i+1) {
            p[i].x = -1;
            p[i].y = -1;
        }
    }
    fclose(file);
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
                for (long i = 0; i < 4147480; i++)
                {
                    write_black_pixel(buffer, i);
                }

                // struct Point p;
                // p.x = 100;
                // p.y = 250;
                // int r = 50;
                // draw_circle(p, r);
                // struct Point p2;
                // p2.x = 200;
                // p2.y = 300;
                // draw_rect(p, p2);
                read_file_sqr("sqr.txt");
                read_file_crc("crc.txt");
                // read_file_pol_three("pol_three.txt");
                read_file_pol_four("pol_four.txt");
                read_file_pol_five("pol_five.txt");
                read_file_pol_six("pol_six.txt");
                // FILE *file = fopen("gunung.txt", "r");
                // char line[25];

                // while (fgets(line, sizeof(line), file))
                // {
                //     int i = 0;
                //     int j = 0;
                //     int k = 0;
                //     long temp[2];
                //     struct Point points[2];
                //     points[0].x = 0;
                //     points[0].y = 0;
                //     points[1].x = 0;
                //     points[1].y = 0;
                //     temp[0] = 0;
                //     temp[1] = 0;
                //     while (line[i] != '\n')
                //     {
                //         if (line[i] == ' ')
                //         {
                //             points[k].x = temp[0];
                //             points[k].y = temp[1];
                //             temp[0] = 0;
                //             temp[1] = 0;
                //             k++;
                //             j = 0;
                //         }
                //         else if (line[i] == ',')
                //         {
                //             j++;
                //         }
                //         else
                //         {
                //             temp[j] = temp[j] * 10 + (line[i] - '0');
                //         }
                //         i++;
                //     }
                //     points[k].x = temp[0];
                //     points[k].y = temp[1];
                //     // printf("1:");
                //     // write_point(points[0]);
                //     // printf("2:");
                //     // write_point(points[1]);
                //     if (points[0].x <= points[1].x)
                //     {
                //         print_line(points[0], points[1]);
                //     }
                //     else
                //     {
                //         print_line(points[1], points[0]);
                //     }
                // }

                // fclose(file);

                // for (long i = 0; i < 19000; i++)
                // {
                //     move_pixels();
                // }
            }
        }
    }
    if (buffer && buffer != MAP_FAILED)
        munmap(buffer, buflen);
    if (fd >= 0)
        close(fd);

    return r;
}
