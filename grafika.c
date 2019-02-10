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
polygon_array polygon_arr;
circle_array circle_arr;
rect_array rect_arr;

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
void draw_circle(circle c)
{
    int x = 0;
    int y = c.r;
    int d = 3 - 2 * c.r;
    draw_all_quadrant(c.p, x, y);
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
        draw_all_quadrant(c.p, x, y);
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
void draw_rect(rect r) {
    Point p3, p4;
    p3.x = r.p1.x;
    p3.y = r.p2.y;
    p4.x = r.p2.x;
    p4.y = r.p1.y;
    print_line(r.p2, p3);
    print_line(p3, r.p1);
    print_line(r.p1, p4);
    print_line(p4, r.p2);
}

void read_file_sqr(char* filename) {
	FILE *file = fopen(filename, "r");
  rect_arr.rects = malloc(sizeof(rect)*20);
  int i = 0;

	while(fscanf(file, "%d,%d %d,%d", &rect_arr.rects[i].p1.x, &rect_arr.rects[i].p1.y, &rect_arr.rects[i].p2.x, &rect_arr.rects[i].p2.y) != EOF){
        i++;
	}
	fclose(file);
  rect_arr.N = i;
}

void read_file_polygon(char* filename) {
  FILE *file = fopen(filename, "r");
  polygon_arr.polygons = malloc(sizeof(polygon)*20);
  int i=0;
  int ip;

  while(fscanf(file, "%d", &polygon_arr.polygons[i].N) != EOF){
    polygon_arr.polygons[i].points = malloc(sizeof(Point)*polygon_arr.polygons[i].N);
    for (ip=0; ip<polygon_arr.polygons[i].N; ip++) {
      fscanf(file, "%d,%d", &polygon_arr.polygons[i].points[ip].x, &polygon_arr.polygons[i].points[ip].y);
    }
    i++;
  }
  fclose(file);
  polygon_arr.N = i;
}

void read_file_crc(char* filename) {
	FILE *file = fopen(filename, "r");
  circle_arr.circles = malloc(sizeof(circle)*20);

    int i = 0;
	while(fscanf(file, "%d,%d %d", &circle_arr.circles[i].p.x, &circle_arr.circles[i].p.y, &circle_arr.circles[i].r) != EOF){
        i++;
	}
	fclose(file);
  circle_arr.N = i;
}

void clear_screen(){
    for (long i = 0; i < 4147480; i++)
    {
        write_black_pixel(buffer, i);
    }
    pixel_count = 0;
}

void translate_r(rect re, int dx, int dy){

    int c = dx / dy;

    for(int i = 1; i <= dx; i++)
    {
        // clear_screen();
        re.p1.x += 1;
        re.p2.x += 1;
        draw_rect(re);
    }

}

void translate_circle(circle crc, int dx, int dy) {

    int init_x = crc.p.x;
    int init_y = crc.p.y;
    int addY = dy / dx;
    for (int i = 0; i < dx; i++) {
        clear_screen();
        crc.p.x += 1;
        crc.p.y += addY;
        draw_circle(crc);
    }
    clear_screen();
    crc.p.x = init_x + dx;
    crc.p.y = init_y + dy;
    draw_circle(crc);
}

void dilate_circle(circle crc, float multiplier) {
    int rad = (int) (crc.r * multiplier);

    if (crc.r <= rad ) {
        for (int i = crc.r; i<=rad; i++) {
            clear_screen();
            crc.r = i;
            draw_circle(crc);
        }
    } else {
        for (int i = crc.r; i >= rad; i--) {
            clear_screen();
            crc.r = i;
            draw_circle(crc);
        }
    }
}

void draw_all() {
  for (int i=0; i<rect_arr.N; i++) {
    draw_rect(rect_arr.rects[i]);
  }

  for (int i=0; i<circle_arr.N; i++) {
    draw_circle(circle_arr.circles[i]);
  }

  for (int i=0; i<polygon_arr.N; i++) {
    draw_polygon(polygon_arr.polygons[i]);
  }
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
                clear_screen();

                read_file_polygon("polygon.txt");
                read_file_crc("crc.txt");
                read_file_sqr("sqr.txt");

                draw_all();
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
