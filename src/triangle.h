#if !defined(TRIANGLE_H)
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"
#include "texture.h"

typedef struct
{
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    Color_ui32 color;
} face_t;

typedef struct
{
    vec4_t points[3];
    tex2_t texcoords[3];
    Color_ui32 color;
    float avg_depth;
} triangle_t;

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);
void draw_texel(int x, int y, Color_ui32 *texture,
                vec4_t point_a, vec4_t point_b, vec4_t point_c,
                tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);
void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0,
                            int x1, int y1, float z1, float w1, float u1, float v1,
                            int x2, int y2, float z2, float w2, float u2, float v2,
                            Color_ui32 *texture);

#endif // TRIANGLE_H
