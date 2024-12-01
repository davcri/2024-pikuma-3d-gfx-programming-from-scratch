#include <stdio.h>
#include "texture.h"

int texture_width = 64;
int texture_height = 64;

Color_ui32 *mesh_texture = NULL;

// rgba in [0, 255] range
uint32_t createColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | (uint32_t)a;
}

// rgba in [0, 255] range
uint32_t createColorARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}
void load_png_texture_data(char *filename)
{

    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return;
    }

    // Create libpng read structs
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        fclose(fp);
        fprintf(stderr, "Error: Unable to create png read struct\n");
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        fprintf(stderr, "Error: Unable to create png info struct\n");
        return;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        fprintf(stderr, "Error: libpng encountered an error\n");
        return;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
    {
        png_set_strip_16(png); // Convert 16-bit depth to 8-bit
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png); // Expand palette images to RGB
    }

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    {
        png_set_expand_gray_1_2_4_to_8(png); // Expand grayscale to 8-bit
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png); // Convert transparency to alpha
    }

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY)
    {
        png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER); // Add full alpha if missing
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png); // Convert grayscale to RGB
    }

    png_read_update_info(png, info);

    // Allocate memory for pixel data
    png_bytep *row_pointers = malloc(sizeof(png_bytep) * height);
    size_t row_size = png_get_rowbytes(png, info);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = malloc(row_size);
    }

    png_read_image(png, row_pointers);
    fclose(fp);

    // Convert raw data to Color_ui32 format
    mesh_texture = malloc(width * height * sizeof(Color_ui32));
    texture_width = width;
    texture_height = height;

    // Load pixel data into mesh_texture
    for (int y = 0; y < height; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++)
        {
            png_bytep px = &row[x * 4];                                              // RGBA: 4 bytes per pixel
            Color_ui32 color = (px[3] << 24) | (px[2] << 16) | (px[1] << 8) | px[0]; // 0xAABBGGRR
            // Color_ui32 color = (px[0] << 24) | (px[1] << 16) | (px[2] << 8) | px[3]; // 0xRRGGBBAA
            mesh_texture[y * width + x] = color;
        }
        free(row);
    }

    free(row_pointers);
    png_destroy_read_struct(&png, &info, NULL);
}
