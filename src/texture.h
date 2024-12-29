#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <png.h>
#include "display.h"

typedef struct TextureCoordinates
{
    float u;
    float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

// extern upng_t *png_texture;
extern Color_ui32 *mesh_texture;

void load_png_texture_data(char *filename);
tex2_t tex2_clone(tex2_t* t);

#endif // TEXTURE_H
