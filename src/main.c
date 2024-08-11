#include <stdbool.h>
#include <SDL.h>

#include "display.h"
#include "vector.h"

const int N_POINTS = 9 * 9 * 9;
vec3_t cube_points[N_POINTS];

bool is_running = false;

void setup(void)
{
    color_buffer = (Color_ui32 *)malloc(sizeof(Color_ui32) * framebuffer_width * framebuffer_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, framebuffer_width, framebuffer_height);

    // initialize cube points
    int point_count = 0;
    for (float x = -1; x < 1; x += 0.25)
    {
        for (float y = -1; y < 1; y += 0.25)
        {
            for (float z = -1; z < 1; z += 0.25)
            {
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[point_count++] = new_point;
            }
        }
    }
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

    draw_grid(0xff888888);
    draw_pixel(0, 0, 0xffff0000);
    // int centerX = (int)round(framebuffer_width * 0.5);
    // int centerY = (int)round(framebuffer_height * 0.5);
    // draw_rectangle(0, 0, centerX, centerY, 0xffff0000);
    // draw_rectangle(centerX, 0, centerX, centerY, 0xffffff00);
    // draw_rectangle(centerX, centerY, centerX, centerY, 0xff0000ff);
    // draw_rectangle(0, centerY, centerX, centerY, 0xffff00ff);

    render_color_buffer();
    SDL_RenderPresent(renderer);
}

int main()
{

    fprintf(stdout, "main(): init\n");
    is_running = initialize_window();
    if (!is_running)
    {
        return 1;
    }

    setup();
    fprintf(stdout, "main(): setup done\n");

    while (is_running)
    {
        process_input();

        update();
        render();
    }

    destroy_window();
    fprintf(stdout, "main(): window destroyed\n");

    return 0;
}