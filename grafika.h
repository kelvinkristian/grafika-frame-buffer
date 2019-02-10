
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

typedef struct RectangleArray
{
  int N;
  rect * rects;
}rect_array;

typedef struct Polygon
{
  int N;
  Point * points;
}polygon;

typedef struct PolygonArray
{
  int N;
  polygon * polygons;
}polygon_array;

typedef struct Circle
{
    Point p;
    int r;
}circle;

typedef struct CircleArray
{
  int N;
  circle * circles;
}circle_array;

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
