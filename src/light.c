#include "light.h"
#include <stdint.h>
#include "display.h"

light_t light = {
    .direction = {0, 0, 1.0},
};

/**
 * Input color: RRGGBBAA
 * Output color: AABBGGRR
 *
 * TODO: improve color management. Worth checking SDL_Color.
 */
Color_ui32 light_apply_intensity(Color_ui32 original_color, float percentage_factor)
{
    // Extract individual components from RRGGBBAA format
    uint8_t red = (original_color >> 24) & 0xFF;
    uint8_t green = (original_color >> 16) & 0xFF;
    uint8_t blue = (original_color >> 8) & 0xFF;
    uint8_t alpha = original_color & 0xFF;

    // Apply the percentage factor and clamp using SDL_clamp
    red = SDL_clamp((int)(red * percentage_factor), 0, 255);
    green = SDL_clamp((int)(green * percentage_factor), 0, 255);
    blue = SDL_clamp((int)(blue * percentage_factor), 0, 255);

    // Reconstruct the color in AABBGGRR format
    return (alpha << 24) | (blue << 16) | (green << 8) | red;
}