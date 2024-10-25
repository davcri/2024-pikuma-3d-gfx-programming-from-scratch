#if !defined(TRIANGLE_H)
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"

typedef struct
{
    int a;
    int b;
    int c;
} face_t;

typedef struct
{
    vec2_t points[3];
} triangle_t;

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);

#endif // TRIANGLE_H
