#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include <stdint.h>
#include "display.h"

typedef struct
{
    vec3_t direction;
} light_t;

Color_ui32 light_apply_intensity(Color_ui32 original_color, float percentage_factor);

extern light_t light;

#endif