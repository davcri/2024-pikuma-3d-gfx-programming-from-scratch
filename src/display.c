#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int window_width = 800;
const int window_height = 600;
int framebuffer_width = window_width;
int framebuffer_height = window_height;
Color_ui32 *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width, window_height,
        SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    int defaultRenderDevice = -1;
    renderer = SDL_CreateRenderer(window, defaultRenderDevice, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    return true;
}

void draw_grid(uint32_t lineColor)
{

    for (int y = 0; y < framebuffer_height; y++)
    {
        for (int x = 0; x < framebuffer_width; x++)
        {
            if (x % 10 == 0 || y % 10 == 0)
            {
                color_buffer[framebuffer_width * y + x] = lineColor;
            }
        }
    }
}

void draw_pixel(int x, int y, Color_ui32 col)
{
    color_buffer[framebuffer_width * y + x] = col;
}

void draw_rectangle(int topLeftX, int topLeftY, int width, int height, uint32_t fillColor)
{
    for (int y = imax(0, topLeftY); y < imin(framebuffer_height, topLeftY + height); y++)
    {
        for (int x = imax(0, topLeftX); x < imin(framebuffer_width, topLeftX + width); x++)
        {
            if (x >= 0 && y >= 0 && x < framebuffer_width && y < framebuffer_height)
            {
                color_buffer[framebuffer_width * y + x] = fillColor;
            }
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, Color_ui32 color)
{
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    // float m = delta_y / delta_x;

    int side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length; // the slope

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++)
    {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

/**
 * Unfilled triangle
 */
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, Color_ui32 color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void render_color_buffer(void)
{
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, framebuffer_width * sizeof(uint32_t));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color)
{
    for (int y = 0; y < framebuffer_height; y++)
    {
        for (int x = 0; x < framebuffer_width; x++)
        {
            color_buffer[framebuffer_width * y + x] = color;
        }
    }
}

void destroy_window(void)
{
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_RenderPresent(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// utils

int imax(float a, float b)
{
    return (int)round(fmax(a, b));
}

int imin(float a, float b)
{
    return (int)round(fmin(a, b));
}
