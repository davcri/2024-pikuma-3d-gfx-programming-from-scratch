#include "triangle.h"
#include "display.h"
#include "swap.h"

Color_ui32 uv_to_color(float u, float v)
{
    // Clamp UV values to the range [0, 1] to avoid overflow or underflow
    if (u < 0)
        u = 0;
    if (u > 1)
        u = 1;
    if (v < 0)
        v = 0;
    if (v > 1)
        v = 1;

    // Convert UV values to RGB components
    Color_ui8 red = (Color_ui8)(u * 255);   // U -> Red
    Color_ui8 green = (Color_ui8)(v * 255); // V -> Green
    Color_ui8 blue = 0;                     // Optional, set to a constant
    Color_ui8 alpha_channel = 255;          // Fully opaque

    // Pack components into AARRGGBB format
    return (alpha_channel << 24) | (red << 16) | (green << 8) | blue;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
// Note: y grows going down.
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color)
{
    // vertices need to be sorted by y so that y0 < y1 < y2
    if (y0 > y1)
    {
        // swap so that y0 < y1
        swap_int(&y0, &y1);
        swap_int(&x0, &x1);
    }
    if (y1 > y2)
    {
        swap_int(&y1, &y2);
        swap_int(&x1, &x2);
    }
    if (y0 > y1)
    {
        swap_int(&y0, &y1);
        swap_int(&x0, &x1);
    }

    if (y1 == y2)
    {
        // we can simply draw the flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    else if (y0 == y1)
    {
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    else
    {
        // calculate the new midpoint vertex (Mx, My) using triangle similarity
        int My = y1;
        int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights alpha, beta, and gamma for point p
///////////////////////////////////////////////////////////////////////////////
//
//         (B)
//         /|\
//        / | \
//       /  |  \
//      /  (P)  \
//     /  /   \  \
//    / /       \ \
//   //           \\
//  (A)------------(C)
//
///////////////////////////////////////////////////////////////////////////////
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    // Find the vectors between the vertices ABC and point p
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);

    // Compute the area of the full parallelogram/triangle ABC using 2D cross product
    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

    // Alpha is the area of the small parallelogram/triangle PBC divided by the area of the full parallelogram/triangle ABC
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

    // Beta is the area of the small parallelogram/triangle APC divided by the area of the full parallelogram/triangle ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

    // Weight gamma is easily found since barycentric coordinates always add up to 1.0
    float gamma = 1 - alpha - beta;

    vec3_t weights = {alpha, beta, gamma};
    return weights;
}

const bool DEBUG_UV = false;

// Draw textured pixel at position xy
void draw_texel(int x, int y, Color_ui32 *texture,
                vec4_t point_a, vec4_t point_b, vec4_t point_c,
                tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
// float u0, float v0, float u1, float v1, float u2, float v2)
{
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Variables to store the interpolated values of U, V, and also w for the current panel
    float interpolated_u;
    float interpolated_v;
    float interpolate_reciprocal_w;

    // Perform interpolation of U and V values using barycentric weights
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    // Also interpolate the value of 1/w for the current pixel
    interpolate_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Divide back both interpolated values by 1/w
    interpolated_u /= interpolate_reciprocal_w;
    interpolated_v /= interpolate_reciprocal_w;

    // Map UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));

    if (DEBUG_UV)
    {
        // Pack color into AARRGGBB format
        Color_ui32 uv_color = uv_to_color(interpolated_u, interpolated_v);
        draw_pixel(x, y, uv_color);
        return;
    }
    else
    {
        // Fetch texel color
        Color_ui32 texel_color = texture[texture_width * tex_y + tex_x];

        // Draw texel to frame buffer
        draw_pixel(x, y, texel_color);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float slope_inv1 = (float)(x1 - x0) / (y1 - y0);
    float slope_inv2 = (float)(x2 - x0) / (y2 - y0);

    float x_start = x0;
    float x_end = x0;

    for (int y = y0; y <= y2; y++)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start += slope_inv1;
        x_end += slope_inv2;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color)
{
    float slope_inv1 = (float)(x2 - x0) / (y2 - y0);
    float slope_inv2 = (float)(x2 - x1) / (y2 - y1);

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y1; y--)
    {
        // printf("%d %d\n", x_start, x_end);
        draw_line(x_start, y, x_end, y, color); // draw scanline
        x_start -= slope_inv1;
        x_end -= slope_inv2;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------v3
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    Color_ui32 *texture)
{
    // Sort the vertices by the y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1)
    {
        // swap so that y0 < y1
        swap_int(&y0, &y1);
        swap_int(&x0, &x1);
        swap_float(&z0, &z1);
        swap_float(&w0, &w1);
        swap_float(&u0, &u1);
        swap_float(&v0, &v1);
    }
    if (y1 > y2)
    {
        swap_int(&y1, &y2);
        swap_int(&x1, &x2);
        swap_float(&z1, &z2);
        swap_float(&w1, &w2);
        swap_float(&u1, &u2);
        swap_float(&v1, &v2);
    }
    if (y0 > y1)
    {
        swap_int(&y0, &y1);
        swap_int(&x0, &x1);
        swap_float(&z0, &z1);
        swap_float(&w0, &w1);
        swap_float(&u0, &u1);
        swap_float(&v0, &v1);
    }

    // Create vector points after sorting the vertices
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

    // Render the upper part of the triangle (flat-bottom)
    float inv_slope_1 = 0.;
    float inv_slope_2 = 0.;

    if (y1 - y0 != 0)
        inv_slope_1 = (float)(x1 - x0) / (y1 - y0);

    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    if (y1 - y0 != 0)
    {
        for (int y = y0; y < y1; y++)
        {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start)
            {
                swap_int(&x_end, &x_start); // Swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++)
            {
                // Draw pixel from the color from the texture
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    // Render the bottom part of the triangle
    inv_slope_1 = 0.;
    inv_slope_2 = 0.;

    if (y2 - y1 != 0)
        inv_slope_1 = (float)(x2 - x1) / (y2 - y1);

    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    if (y2 - y1 != 0)
    {
        for (int y = y1; y < y2; y++)
        {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start)
            {
                swap_int(&x_end, &x_start); // Swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++)
            {
                // Draw pixel from the color from the texture
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
}
