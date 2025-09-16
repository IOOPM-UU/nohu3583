#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct point
{
  int x;
  int y;
};

typedef struct point point_t;

void translate(point_t *p1, point_t *p2)
{
  p1->x += p2->x;
  p1->y += p2->y;
}

void print_point(const point_t *p)
{
  printf("point(%d, %d)", p->x, p->y);
}

void make_point(point_t *p, int x, int y)
{
  p->x = x;
  p->y = y;
}

struct rectangle
{
  point_t upper_left;
  point_t lower_right;
};

typedef struct rectangle rectangle_t;


void print_rec(const rectangle_t *r)
{
    printf("rectangle(upper_left=");
    print_point(&r->upper_left);
    printf(", lower_right=");
    print_point(&r->lower_right);
    printf(")\n");
    return;
}

void make_rec(rectangle_t *r, int upper_left_x, int upper_left_y, int lower_right_x, int lower_right_y) {
    make_point(&r->upper_left, upper_left_x, upper_left_y);
    make_point(&r->lower_right, lower_right_x, lower_right_y);

}
int area_rec(const rectangle_t *r) {
    int width = r->lower_right.x - r->upper_left.x;
    int height = r->lower_right.y - r->upper_left.y;
    return width * height;
}

bool interesct_rec(const rectangle_t *r1, const rectangle_t *r2) {
    // If one rectangle is to the left of the other
    if (r1->lower_right.x <= r2->upper_left.x || r2->lower_right.x <= r1->upper_left.x)
        return false;
    // If one rectangle is above the other
    if (r1->upper_left.y >= r2->lower_right.y || r2->upper_left.y >= r1->lower_right.y)
        return false;
    return true;
}

int main(void)
{
    point_t p1, p2;
    make_point(&p1, 1, 0);
    make_point(&p2, 3, 4);
    print_point(&p1);
    printf("\n");
    print_point(&p2);
    printf("\n");
    
    rectangle_t r;
    make_rec(&r, p1.x, p1.y, p2.x, p2.y);
    print_rec(&r);
    printf("Area: %d\n", area_rec(&r));
    return 0;

}