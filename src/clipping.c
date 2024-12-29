#include <math.h>
#include "clipping.h"

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | |
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | |
//           \|/
//
///////////////////////////////////////////////////////////////////////////////
void init_frustum_planes(float fovx, float fovy, float z_near, float z_far)
{
    float cos_half_fovx = cos(fovx / 2);
    float sin_half_fovx = sin(fovx / 2);
    float cos_half_fovy = cos(fovy / 2);
    float sin_half_fovy = sin(fovy / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fovx;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fovx;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fovy;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fovy;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2)
{
    polygon_t polygon = {
        .vertices = {v0, v1, v2},
        .texcoords = {t0, t1, t2},
        .num_vertices = 3,
    };

    return polygon;
}

float lerp_float(float a, float b, float t) {
    return a + t * (b - a);
}

void clip_polygon_against_plane(polygon_t *poly, int plane)
{
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    // Declare a static array of inside vertices
    vec3_t inside_vertices[MAX_NUM_POLY_VERTICES];
    tex2_t inside_texcoords[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    // Get the current and previous vertices
    vec3_t *current_vertex = &poly->vertices[0];
    tex2_t *current_texcoord = &poly->texcoords[0];

    // Initialize the previous vertex with the last polygon vertex and texture coordinate
    vec3_t *previous_vertex = &poly->vertices[poly->num_vertices - 1];
    tex2_t *previous_texcoord = &poly->texcoords[poly->num_vertices - 1];

    // Calculate dot products
    float dot_current = 0;
    float dot_previous = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

    // Loop all the polygon vertices while the current is different than the last one
    while (current_vertex != &poly->vertices[poly->num_vertices])
    {
        dot_current = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

        // If we changed from inside to outside or from outside to inside
        if (dot_current * dot_previous < 0)
        {
            // Find the interpolation factor t
            float t = dot_previous / (dot_previous - dot_current);

            // Calculate the intersection point
            vec3_t intersection_point = {
                .x = lerp_float(previous_vertex->x, current_vertex->x, t),
                .y = lerp_float(previous_vertex->y, current_vertex->y, t),
                .z = lerp_float(previous_vertex->z, current_vertex->z, t),
            };
                
            tex2_t interpolated_texcoord = {
                .u = lerp_float(previous_texcoord->u, current_texcoord->u, t),
                .v = lerp_float(previous_texcoord->v, current_texcoord->v, t),
            };

            // Insert the current vertex to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            inside_texcoords[num_inside_vertices] = tex2_clone(&interpolated_texcoord);
            num_inside_vertices++;
        }

        // If current vertex is inside the plane
        if (dot_current > 0)
        {
            // Insert the current vertex to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);

            num_inside_vertices++;
        }

        // Move to the next
        dot_previous = dot_current;
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
        current_vertex++;
        current_texcoord++;
    }

    // Copy the list of inside vertices into the destination polygon
    for (int i = 0; i < num_inside_vertices; i++)
    {
        poly->vertices[i] = vec3_clone(&inside_vertices[i]);
        poly->texcoords[i] = tex2_clone(&inside_texcoords[i]);
    }
    poly->num_vertices = num_inside_vertices;
}

void triangles_from_polygon(polygon_t *polygon, triangle_t triangles[], int *num_triangles)
{
    for (int i = 0; i < polygon->num_vertices - 2; i++)
    {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(polygon->vertices[index0]);
        triangles[i].points[1] = vec4_from_vec3(polygon->vertices[index1]);
        triangles[i].points[2] = vec4_from_vec3(polygon->vertices[index2]);

        triangles[i].texcoords[0] = polygon->texcoords[index0];
        triangles[i].texcoords[1] = polygon->texcoords[index1];
        triangles[i].texcoords[2] = polygon->texcoords[index2];
    }
    *num_triangles = polygon->num_vertices - 2;
}

void clip_polygon(polygon_t *poly)
{
    clip_polygon_against_plane(poly, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, FAR_FRUSTUM_PLANE);
}
