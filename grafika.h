
typedef struct Point
{
    int x;
    int y;
}Point;

typedef struct Line
{
    Point p1;
    Point p2;
}line;

typedef struct Rectangle
{
    Point p1;
    Point p2;
}rect;

typedef struct Polygon
{
  int N;
  Point * points;
}polygon;

typedef struct Circle
{
    Point c;
    int r;
}circle;

typedef union
{
    rect r;
    circle c;
}object;

enum type {
    RCT,
    CRC,
    POL
};
