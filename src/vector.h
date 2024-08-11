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

#endif // VECTOR2_H
