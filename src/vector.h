#if !defined(VECTOR2_H)
#define VECTOR2_H

typedef struct
{
    float x;
    float y;
} vec2_t;

typedef struct
{
    float x;
    float y;
    float z;
} vec3_t;

typedef struct
{
    vec3_t position;
    vec3_t rotation; // this should be a quaternion, but for simplicity it will be an xyz Euler
    float fov;
} camera_t;

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

#endif // VECTOR2_H
