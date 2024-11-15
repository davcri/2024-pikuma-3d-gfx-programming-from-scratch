#include "light.h"
#include <stdint.h>
#include "display.h"

light_t light = {
    .direction = {0, 0, 1.0},
};

Color_ui32 light_apply_intensity(Color_ui32 original_color, float percentage_factor)
{
    if (percentage_factor < 0)
        percentage_factor = 0;
    if (percentage_factor > 1)
        percentage_factor = 1;

    uint32_t a = (original_color & 0xff000000);
    uint32_t r = (original_color & 0x00ff0000) * percentage_factor;
    uint32_t g = (original_color & 0x0000ff00) * percentage_factor;
    uint32_t b = (original_color & 0x000000ff) * percentage_factor;

    return a | (r & 0x00ff0000) | (g & 0x0000ff00) | (b & 0x000000ff);
}