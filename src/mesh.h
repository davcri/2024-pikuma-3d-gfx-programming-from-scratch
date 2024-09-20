#if !defined(MESH_H)
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES 6 * 2

extern vec3_t mesh_vertices[N_CUBE_VERTICES];
extern face_t mesh_faces[N_CUBE_FACES];

void load_cube_mesh_data();
void load_obj_file_data(char *filename);

typedef struct
{
    vec3_t *vertices;
    face_t *faces;
    vec3_t rotation; // euler rotation
} mesh_t;

extern mesh_t mesh; // global mesh

#endif // MESH_H