#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

int imax(float a, float b)
{
    return (int)round(fmax(a, b));
}

int imin(float a, float b)
{
    return (int)round(fmin(a, b));
}
bool is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int window_width = 800;
const int window_height = 600;
int framebuffer_width = 4;  // window_width / 10;
int framebuffer_height = 4; // window_height / 10;
uint32_t *color_buffer = NULL;
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

void render_color_buffer(void)
{
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, framebuffer_width * sizeof(uint32_t));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
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

void destroy_window(void)
{
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_RenderPresent(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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