#if !defined(TEXTURE_H)
#define TEXTURE_H

#include "display.h"

typedef struct TextureCoordinates
{
    float u;
    float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

extern const Color_ui8 REDBRICK_TEXTURE[];

extern Color_ui32 *mesh_texture;

#endif // TEXTURE_H
