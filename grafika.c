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

// Using macros to convert degree to radian
// and call sin() and cos() as these functions
// take input in radians
#define SIN(x) sin(x * 3.141592653589/180)
#define COS(x) cos(x * 3.141592653589/180)

struct fb_var_screeninfo screen_info;
struct fb_fix_screeninfo fixed_info;


char *buffer = NULL;
long pixels[10000];
int pixel_count = 0;
polygon_array polygon_arr;
circle_array circle_arr;
rect_array rect_arr;

void draw_all();

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
    // for (long i = 0; i < 4147480; i+=4)
    // {
    //     write_black_pixel(buffer, i);
    // }
    memset(buffer, 0x00, 4147480);
    pixel_count = 0;
}

void translate_r(rect *re, int dx, int dy){

    Point init_p1 = re->p1;
    Point init_p2 = re->p2;
    float addY = (float)dy / (float)dx;

    for(int i = 0; i < dx/4; i++)
    {
        re->p1.x += 4;
        re->p1.y += addY*4;
        re->p2.x += 4;
        re->p2.y += addY*4;
        draw_all();
    }
    re->p1.x = init_p1.x+dx;
    re->p2.x = init_p2.x+dx;
    re->p1.y = init_p1.y+dy;
    re->p2.y = init_p2.y+dy;
    draw_all();
}

void dilate_r(rect *re, float multiplier){
    int p = re->p2.x - re->p1.x;
    int l = re->p2.y - re->p1.y;
    float control = (float)l/(float)p;

    for(int i = 0;i < p/4;i++){
        re->p2.x += 4;
        re->p2.y += control*4;
        draw_all();
    }
}

void translate_circle(circle *crc, int dx, int dy) {

    int init_x = crc->p.x;
    int init_y = crc->p.y;
    float addY = (float)dy / (float)dx;
    for (int i = 0; i < dx/4; i++) {
        crc->p.x += 4;
        crc->p.y += addY*4;
        draw_all();
    }
    crc->p.x = init_x + dx;
    crc->p.y = init_y + dy;
    draw_all();
}

void dilate_circle(circle *crc, float multiplier) {
    int rad = (int) (crc->r * multiplier);

    if (crc->r <= rad ) {
        for (int i = crc->r; i<=rad; i++) {
            crc->r = i;
            draw_all();
        }
    } else {
        for (int i = crc->r; i >= rad; i--) {
            crc->r = i;
            draw_all();
        }
    }
}

void draw_all() {
  clear_screen();
  // sleep(1);
  for (int i=0; i<polygon_arr.N; i++) {
    draw_polygon(polygon_arr.polygons[i]);
  }

  for (int i=0; i<rect_arr.N; i++) {
    draw_rect(rect_arr.rects[i]);
  }

  for (int i=0; i<circle_arr.N; i++) {
    draw_circle(circle_arr.circles[i]);
  }
  usleep(30000);
}

void translate_polygon(polygon p, int dx, int dy) {
    polygon temp = p;
    int addY = dy/dx;
    for (int i = 0; i < dx; i++) {
        clear_screen();
        for (int j=0; j <=p.N; j++) {
            p.points[i].x++;
            p.points[i].y += addY;
        }
        draw_polygon(p);
    }
    clear_screen();
    //for (int j=0; j <=p.N; j++) {
      //  p.points[i].x = temp.points[i].x + dx;
        //p.points[i].y = temp.points[i].y + dy;
    //}
    //draw_polygon(p);
}

void rotate(float a[][2], int n, int x_pivot,
                      int y_pivot, int angle)
{
    int i = 0;
    while (i < n)
    {
        // Shifting the pivot point to the origin
        // and the given points accordingly
        int x_shifted = a[i][0] - x_pivot;
        int y_shifted = a[i][1] - y_pivot;

        // Calculating the rotated point co-ordinates
        // and shifting it back
        a[i][0] = x_pivot + (x_shifted*COS(angle)
                          - y_shifted*SIN(angle));
        a[i][1] = y_pivot + (x_shifted*SIN(angle)
                          + y_shifted*COS(angle));
        printf("(%f, %f) ", a[i][0], a[i][1]);
        i++;
    }
}

void rotate_polygon(struct Polygon polygon, Point pivot, int angle) {
    int i = 0;
    while (i < polygon.N) {
      int x_shifted = polygon.points[i].x - pivot.x;
      int y_shifted = polygon.points[i].y - pivot.y;
      polygon.points[i].x = pivot.x + (x_shifted*COS(angle) - y_shifted*SIN(angle));
      polygon.points[i].y = pivot.y + (x_shifted*SIN(angle) + y_shifted*COS(angle));
      //	clear_screen();
    //	draw_polygon(polygon);
      i++;
    }
    draw_polygon(polygon);
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

                translate_r(&rect_arr.rects[0], 100,100);
                dilate_r(&rect_arr.rects[0], 2);
                translate_circle(&circle_arr.circles[0], 200,200);
                dilate_circle(&circle_arr.circles[0], 2);
            }
        }
    }
    if (buffer && buffer != MAP_FAILED)
        munmap(buffer, buflen);
    if (fd >= 0)
        close(fd);

    return r;
}
