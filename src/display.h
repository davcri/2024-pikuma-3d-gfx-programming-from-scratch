#if !defined(DISPLAY_H) // https://learn.microsoft.com/en-us/cpp/preprocessor/hash-ifdef-and-hash-ifndef-directives-c-cpp?view=msvc-170
#define DISPLAY_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

extern SDL_Window *window; // https://stackoverflow.com/questions/496448/how-to-correctly-use-the-extern-keyword-in-c
extern SDL_Renderer *renderer;
extern const int window_width;
extern const int window_height;
extern int framebuffer_width;
extern int framebuffer_height;
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;

bool initialize_window(void);
void draw_grid(uint32_t lineColor);
void draw_rectangle(int topLeftX, int topLeftY, int width, int height, uint32_t fillColor);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);

// utils
int imax(float a, float b);
int imin(float a, float b);

#endif // DISPLAY_H
