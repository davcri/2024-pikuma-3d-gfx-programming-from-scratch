#include <stdbool.h>
#include <SDL.h>

#include "display.h"
#include "vector.h"

const int N_POINTS = 9 * 9 * 9;
const int fov_factor = 640;

int previous_frame_time;

vec3_t camera_position = {.x = 0., .y = 0., .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0., .z = 0.};
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

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

vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        .x = fov_factor * point.x / point.z,
        .y = fov_factor * point.y / point.z};
    return projected_point;
}

void update(void)
{
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    for (int i = 0; i < N_POINTS; i++)
    {
        vec3_t point = cube_points[i];
        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

        transformed_point.z -= camera_position.z;
        vec2_t projected_point = project(transformed_point);
        projected_points[i] = projected_point;
    }
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
    // draw_grid(0xff888888);

    // render all projected points
    for (int i = 0; i < N_POINTS; i++)
    {
        vec2_t point = projected_points[i];
        // translate to the middle of the screen
        point.x += window_width / 2;
        point.y += window_height / 2;
        // draw
        draw_rectangle(point.x, point.y, 4, 4, 0xffff0000);
    }

    render_color_buffer();
    clear_color_buffer(0xff000000);

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