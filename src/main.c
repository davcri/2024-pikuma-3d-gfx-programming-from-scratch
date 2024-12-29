#include <stdbool.h>
#include <SDL.h>

#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "texture.h"
#include "triangle.h"
#include "light.h"
#include "mesh.h"
#include "camera.h"
#include "clipping.h"

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool is_running = false;
int previous_frame_time = 0;

mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;
float delta_time;

void setup(void)
{
    render_method = RENDER_TEXTURED;
    cull_method = CULL_BACKFACE;

    color_buffer = (Color_ui32 *)malloc(sizeof(Color_ui32) * framebuffer_width * framebuffer_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
        framebuffer_width, framebuffer_height);
    z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);

    // Load a model
    // load_cube_mesh_data();
    // load_obj_file_data("./assets/f22.obj");
    // load_obj_file_data("./assets/suzanne.obj");
    // load_obj_file_data("./assets/f22.obj");
    load_obj_file_data("./assets/cube.obj");

    // Load the texture data
    load_png_texture_data("./assets/cube.png");

    float aspectx = (float)window_width / (float)window_height;
    float aspecty = (float)window_height / (float)window_width;
    float fovy = M_PI / 3.0; // radians
    float fovx = atan(tan(fovy / 2) * aspectx) * 2.0;
    float z_near = 1;
    float z_far = 20.;
    proj_matrix = mat4_make_perspective(fovy, aspecty, z_near, z_far);

    // Initialize frustum planes
    init_frustum_planes(fovx, fovy, z_near, z_far);
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
        {
            render_method = RENDER_WIRE_VERTEX;
            printf("Render method: %d\n", render_method);
        }
        if (event.key.keysym.sym == SDLK_2)
        {
            render_method = RENDER_WIRE;
            printf("Render method: %d\n", render_method);
        }
        if (event.key.keysym.sym == SDLK_3)
        {
            render_method = RENDER_FILL_TRIANGLE;
            printf("Render method: %d\n", render_method);
        }
        if (event.key.keysym.sym == SDLK_4)
        {
            render_method = RENDER_FILL_TRIANGLE_WIRE;
            printf("Render method: %d\n", render_method);
        }
        if (event.key.keysym.sym == SDLK_5)
        {
            render_method = RENDER_TEXTURED;
            printf("Render method: %d\n", render_method);
        }
        if (event.key.keysym.sym == SDLK_6)
        {
            render_method = RENDER_TEXTURED_WIRE;
            printf("Render method: %d\n", render_method);
        }
        if (event.key.keysym.sym == SDLK_c)
        {
            cull_method = CULL_BACKFACE;
            printf("Cull: %d\n", cull_method);
        }
        if (event.key.keysym.sym == SDLK_x)
        {
            cull_method = CULL_NONE;
            printf("Cull: %d\n", cull_method);
        }
        if (event.key.keysym.sym == SDLK_UP)
            camera.position.y += 3.0 * delta_time;
        if (event.key.keysym.sym == SDLK_DOWN)
            camera.position.y -= 3.0 * delta_time;
        if (event.key.keysym.sym == SDLK_a)
            camera.yaw += 1. * delta_time;
        if (event.key.keysym.sym == SDLK_d)
            camera.yaw -= 1. * delta_time;
        if (event.key.keysym.sym == SDLK_w)
        {
            camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
            camera.position = vec3_add(camera.position, camera.forward_velocity);
        }
        if (event.key.keysym.sym == SDLK_s)
        {
            camera.forward_velocity = vec3_mul(camera.direction, -5.0 * delta_time);
            camera.position = vec3_add(camera.position, camera.forward_velocity);
        }

        break;

    default:
        break;
    }
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

    Uint32 updateTicks = SDL_GetTicks();

    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0; // seconds
    previous_frame_time = SDL_GetTicks();

    // Initialize the counter of triangles to render for the current frame
    num_triangles_to_render = 0;

    // Change the mesh scale/rotation values per frame
    // mesh.rotation.x += 0.004;
    // mesh.rotation.y += 0.8 * delta_time;
    // mesh.rotation.z += 0.004;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;
    // mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    // Create a scale/translation/rotation matrix
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // Calculate camera target
    vec3_t target = {0, 0, 1};
    mat4_t rotation_matrix = mat4_make_rotation_y(camera.yaw);
    camera.direction = vec3_from_vec4(mat4_mul_vec4(rotation_matrix, vec4_from_vec3(target)));
    target = vec3_add(camera.position, camera.direction);

    // Calculate the view matrix
    vec3_t up_direction = {0, 1, 0};
    view_matrix = mat4_look_at(camera.position, target, up_direction);

    // Loop all faces
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        // Transform vertices of face
        vec4_t transformed_vertices[3];
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a world matrix that combines translation/rotation/scale
            world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Apply the world matrix
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Apply the view matrix
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            // save the current vertex
            transformed_vertices[j] = transformed_vertex;
        }

        // prepare data for back-face culling
        vec3_t vec_a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vec_b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vec_c = vec3_from_vec4(transformed_vertices[2]);

        vec3_t vec_ab = vec3_sub(vec_b, vec_a); // b - a
        vec3_t vec_ac = vec3_sub(vec_c, vec_a); // c - a

        vec3_normalize(&vec_ab);
        vec3_normalize(&vec_ac);

        // compute the face normal
        vec3_t face_normal = vec3_cross(vec_ab, vec_ac);
        vec3_normalize(&face_normal);

        vec3_t origin = {0, 0, 0};
        vec3_t camera_ray = vec3_sub(origin, vec_a);

        // calculate how aligned camera ray and face normale are
        float dot_normal_camera = vec3_dot(face_normal, camera_ray);

        if (cull_method == CULL_BACKFACE)
        {
            // perform back-face culling
            if (dot_normal_camera < 0)
            {
                continue;
            }
        }

        polygon_t polygon = polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv);

        // Clip the polygon (may introduce new vertices)
        clip_polygon(&polygon);

        //  Break the clipped polygon apart back into individual tris
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int number_triangles_after_clipping = 0;

        triangles_from_polygon(&polygon, triangles_after_clipping, &number_triangles_after_clipping);

        // Loops all the triangles after clipping
        for (int t = 0; t < number_triangles_after_clipping; t++)
        {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];
            vec4_t projected_points[3];

            // Loop all three vertices to perform projection and conversion to screen space
            for (int j = 0; j < 3; j++)
            {
                // Project
                projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangle_after_clipping.points[j]);

                // Flip Y to account for inverted screen space / model y coordinates
                projected_points[j].y *= -1;

                //
                projected_points[j].x *= (window_width / 2.0);
                projected_points[j].y *= (window_height / 2.0);

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (window_width / 2.0);
                projected_points[j].y += (window_height / 2.0);
            }

            // Shade face
            float light_intensity_factor = -vec3_dot(face_normal, light.direction);
            Color_ui32 triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

            triangle_t triangle_to_render = {
                .points = {
                    {projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
                    {projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
                    {projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w},
                },
                .texcoords = {
                    { triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
                    { triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
                    { triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v},
                },
                .color = triangle_color,
            };

            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH)
            {
                triangles_to_render[num_triangles_to_render] = triangle_to_render;
                num_triangles_to_render++;
            }
        }
    }

    printf("update(): took %dms\n", SDL_GetTicks() - updateTicks);
}

void render(void)
{
    //
    SDL_RenderClear(renderer);

    //
    draw_grid(0xFF444444);

    // render all projected points
    for (int i = 0; i < num_triangles_to_render; i++)
    {
        triangle_t triangle = triangles_to_render[i];
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_filled_triangle(
                // vertex A
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                // vertex B
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                // vertex C
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.color);
        }

        // Draw textured triangle
        if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
        {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture);
        }

        // Draw wireframe
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE)
        {
            draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y, 0xffffffff);
        }

        // render vertices
        if (render_method == RENDER_WIRE_VERTEX)
        {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xffffaaff);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xffffaaff);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xffffaaff);
        }
    }

    render_color_buffer();

    clear_color_buffer(0x00000000);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

void free_resources()
{
    printf("Memory free done.\n");
    free(color_buffer);
    array_free(z_buffer); // This causes an error. TODO: investigate
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
