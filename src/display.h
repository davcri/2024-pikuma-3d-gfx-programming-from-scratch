#if !defined(DISPLAY_H) // https://learn.microsoft.com/en-us/cpp/preprocessor/hash-ifdef-and-hash-ifndef-directives-c-cpp?view=msvc-170
#define DISPLAY_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define FPS 60
#define FRAME_TARGET_TIME 1000.0 / FPS

enum cull_method
{
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum render_method
{
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
} render_method;

typedef uint32_t Color_ui32;

extern SDL_Window *window; // https://stackoverflow.com/questions/496448/how-to-correctly-use-the-extern-keyword-in-c
extern SDL_Renderer *renderer;
extern const int window_width;
extern const int window_height;
extern int framebuffer_width;
extern int framebuffer_height;
extern Color_ui32 *color_buffer;
extern SDL_Texture *color_buffer_texture;

bool initialize_window(void);
void draw_grid(Color_ui32 lineColor);
void draw_rectangle(int topLeftX, int topLeftY, int width, int height, Color_ui32 fillColor);
void draw_pixel(int x, int y, Color_ui32 color);
void draw_line(int x0, int y0, int x1, int y1, Color_ui32 color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color);
void render_color_buffer(void);
void clear_color_buffer(Color_ui32 color);
void destroy_window(void);

// utils
int imax(float a, float b);
int imin(float a, float b);

#endif // DISPLAY_H
