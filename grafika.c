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

rect* read_file_sqr(char* filename) {
	FILE *file = fopen(filename, "r");
    rect* Re = malloc(sizeof(rect)*10);
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

circle* read_file_crc(char* filename) {
	FILE *file = fopen(filename, "r");
    circle* circles = malloc(sizeof(circle)*10);

    int i = 0;
	while(fscanf(file, "%d,%d %d", &circles[i].p.x, &circles[i].p.y, &circles[i].r) != EOF){
        i++;
	}
	fclose(file);
    return circles;
}

void clear_screen(){
    for (long i = 0; i < 4147480; i++)
    {
        write_black_pixel(buffer, i);
    }
    pixel_count = 0;
}

void translate_r(rect *re, int dx, int dy){
    
    Point init_p1 = re->p1;
    Point init_p2 = re->p2;
    float addY = (float)dy / (float)dx;

    for(int i = 0; i < dx/4; i++)
    {
        clear_screen();
        re->p1.x += 4;
        re->p1.y += addY*4;
        re->p2.x += 4;
        re->p2.y += addY*4;
        draw_rect(*re);
    }
    re->p1.x = init_p1.x+dx;
    re->p2.x = init_p2.x+dx;
    re->p1.y = init_p1.y+dy;
    re->p2.y = init_p2.y+dy;
    draw_rect(*re);

}

void dilate_r(rect *re, float multiplier){
    int p = re->p2.x - re->p1.x;
    int l = re->p2.y - re->p1.y;
    float control = (float)l/(float)p;

    for(int i = 0;i < p/4;p++){
        clear_screen();
        re->p2.x += 4;
        re->p2.y += control*4;
        draw_rect(*re);
    }
}

void rotate_rect(rect *r, int angle) {
    Point center;
    center.x = r->p2.x - r->p1.x;
    center.y = r->p2.y - r->p1.y;
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;
    
    for (int a=0; a<=angle; a+=5) {
        clear_screen();
        r->p1.x = (x1-center.x) * COS(a) - (y1-center.y) * SIN(a)+center.x;
        r->p1.y = (x1-center.x) * SIN(a) + (y1-center.y) * COS(a)+center.y;
        r->p2.x = (x2-center.x) * COS(a) - (y2-center.y) * SIN(a)+center.x;
        r->p2.y = (x2-center.x) * SIN(a) + (y2-center.y) * COS(a)+center.y;
        draw_rect(r);
    }
    clear_screen();
    r->p1.x = (x1-center.x) * COS(angle) - (y1-center.y) * SIN(angle)+center.x;
    r->p1.y = (x1-center.x) * SIN(angle) + (y1-center.y) * COS(angle)+center.y;
    r->p2.x = (x2-center.x) * COS(angle) - (y2-center.y) * SIN(angle)+center.x;
    r->p2.y = (x2-center.x) * SIN(angle) + (y2-center.y) * COS(angle)+center.y;
    draw_rect(r);
    

}

void translate_circle(circle *crc, int dx, int dy) {
    
    int init_x = crc->p.x;
    int init_y = crc->p.y;
    float addY = (float)dy / (float)dx;
    for (int i = 0; i < dx/4; i++) {
        clear_screen();
        crc->p.x += 4;
        crc->p.y += addY*4;
        draw_circle(crc->p, crc->r);
    }
    clear_screen();
    crc->p.x = init_x + dx;
    crc->p.y = init_y + dy;
    draw_circle(crc->p, crc->r);
}

void dilate_circle(circle *crc, float multiplier) {
    int rad = (int) (crc->r * multiplier);
    
    if (crc->r <= rad ) {
        for (int i = crc->r; i<=rad; i++) {
            clear_screen();
            crc->r = i;
            draw_circle(crc->p,crc->r);
        }
    } else {
        for (int i = crc->r; i >= rad; i--) {
            clear_screen();
            crc->r = i;
            draw_circle(crc->p, crc->r);
        }
    }
}

void translate_polygon(polygon p, int dx, int dy) {
    Point* temp = malloc(sizeof(Point)*6);
    for (int i = 0; i < 4; i++) {
        temp[i] = p.points[i];
    }
    int addY = dy/dx;
    for (int i = 0; i < dx; i++) {
        clear_screen();
        for (int j=0; j <=p.N; j++) {
            p.points[j].x++;
            p.points[j].y += addY;
        }
        draw_polygon(p);
    }
    clear_screen();
    for (int j=0; j <=p.N; j++) {
        p.points[j].x = temp[j].x + dx;
        p.points[j].y = temp[j].y + dy;
    }
    draw_polygon(p);
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

                polygon* polygon_arr = malloc(sizeof(polygon)*10);
                polygon_arr = read_file_polygon("polygon.txt");
                
                for (int x=0; x<2; x++) {
                  draw_polygon(polygon_arr[x]);
                }
                translate_polygon(polygon_arr[0],10,20);

                Point point;
                point.x = 300;
                point.y = 400;
                rotate_polygon(polygon_arr[0], point, 60);
		//draw_polygon(polygon_arr[0]); 
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
