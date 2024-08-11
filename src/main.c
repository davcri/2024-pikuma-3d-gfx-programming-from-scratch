#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "display.h"

bool is_running = false;

void setup(void)
{
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * framebuffer_width * framebuffer_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, framebuffer_width, framebuffer_height);
}

void update(void)
{
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;
        break;
    default:
        break;
    }
}

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    clear_color_buffer(0xff4a4a4a);
    // draw_grid(0xff888888);
    int centerX = (int)round(framebuffer_width * 0.5);
    int centerY = (int)round(framebuffer_height * 0.5);
    draw_rectangle(0, 0, centerX, centerY, 0xffff0000);
    draw_rectangle(centerX, 0, centerX, centerY, 0xffffff00);
    draw_rectangle(centerX, centerY, centerX, centerY, 0xff0000ff);
    draw_rectangle(0, centerY, centerX, centerY, 0xffff00ff);

    render_color_buffer();

    // SDL_Render
    SDL_RenderPresent(renderer);
}

int main()
{
    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();

        update();
        render();
    }

    destroy_window();

    return 0;
}