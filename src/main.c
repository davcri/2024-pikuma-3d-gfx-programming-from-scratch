#include <stdbool.h>
#include <SDL.h>

#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t *triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time;

vec3_t camera_position = {0., 0., 0};
const int fov_factor = 640;

void setup(void)
{
    render_method = RENDER_FILL_TRIANGLE;
    cull_method = CULL_BACKFACE;

    color_buffer = (Color_ui32 *)malloc(sizeof(Color_ui32) * framebuffer_width * framebuffer_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        framebuffer_width, framebuffer_height);

    load_obj_file_data("./assets/cube.obj");
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
        if (event.key.keysym.sym == SDLK_1)
            render_method = RENDER_WIRE_VERTEX;
        if (event.key.keysym.sym == SDLK_2)
            render_method = RENDER_WIRE;
        if (event.key.keysym.sym == SDLK_3)
            render_method = RENDER_FILL_TRIANGLE;
        if (event.key.keysym.sym == SDLK_4)
            render_method = RENDER_FILL_TRIANGLE_WIRE;
        if (event.key.keysym.sym == SDLK_c)
            cull_method = CULL_BACKFACE;
        if (event.key.keysym.sym == SDLK_d)
            cull_method = CULL_NONE;
        break;

    default:
        break;
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z};
    return projected_point;
}

void update(void)
{
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    float delta = SDL_GetTicks() - previous_frame_time;
    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.y += 0.01;

    // loop faces
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        // transform vertices of face
        for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // translate the vertex away from the camera
            transformed_vertex.z += 5;

            // save the current vertex
            transformed_vertices[j] = transformed_vertex;
        }

        if (cull_method == CULL_BACKFACE)
        {
            // prepare data for baclk-face culling
            vec3_t vec_a = transformed_vertices[0];
            vec3_t vec_b = transformed_vertices[1];
            vec3_t vec_c = transformed_vertices[2];

            vec3_t vec_ab = vec3_sub(vec_b, vec_a); // b - a
            vec3_t vec_ac = vec3_sub(vec_c, vec_a); // c - a

            vec3_normalize(&vec_ab);
            vec3_normalize(&vec_ac);

            // compute the face normal
            vec3_t face_normal = vec3_cross(vec_ab, vec_ac);
            vec3_normalize(&face_normal);

            vec3_t camera_ray = vec3_sub(camera_position, vec_a); //

            // calculate how aligned camera ray and face normale are
            float dot_normal_camera = vec3_dot(face_normal, camera_ray);

            // perform back-face culling
            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        triangle_t projected_triangle;

        // loop all the three vertices to perform projection
        for (int j = 0; j < 3; j++)
        {
            vec2_t projected_point = project(transformed_vertices[j]);

            // scale and translate to the middle of the screen
            projected_point.x += window_width / 2;
            projected_point.y += window_height / 2;

            projected_triangle.points[j] = projected_point;
        }

        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void)
{
    // draw_grid(0xff888888);

    // render all projected points
    Color_ui32 col = 0xffff0000;
    int tris_count = array_length(triangles_to_render);
    for (int i = 0; i < tris_count; i++)
    {
        triangle_t triangle = triangles_to_render[i];
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y, col);
        }

        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y, 0xffffffff);
        }

        if (render_method == RENDER_WIRE_VERTEX)
        {
            draw_rectangle(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xffffaaff);
            draw_rectangle(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xffffaaff);
            draw_rectangle(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xffffaaff);
        }
    }
    array_free(triangles_to_render);

    // draw_filled_triangle(300, 100, 50, 400, 500, 700, col);

    render_color_buffer();

    clear_color_buffer(0xff000000);

    SDL_RenderPresent(renderer);
}

void free_resources()
{
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    fprintf(stdout, "main(): window destroyed\n");

    return 0;
}
