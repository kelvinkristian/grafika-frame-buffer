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
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
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
                for (long i = 0; i < 214748; i++)
                {
                    write_black_pixel(buffer, i);
                }

                struct Point p;
                p.x = 200;
                p.y = 200;
                int r = 50;
                draw_circle(p, r);
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
