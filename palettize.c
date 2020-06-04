
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include "stretchy_buffer.h"

#include "jt_int.h"
#include "jt_loops.h"

#include "vector_math.h"
#include "image.h"

typedef struct {
    size_t count;
    u32 *colors;
    v3  *color_vectors;
} color_palette;

static color_palette load_color_palette(const char *filename) {
    image im = load_image(filename);
    
    if (!im.pixels) {
        fprintf(stderr, "error loading palette image\n");
        exit(EXIT_FAILURE);
    }
    
    u32 *colors        = NULL; // sb
    v3  *color_vectors = NULL; // sb
    
    u32 *end = im.pixels + im.width*im.height;
    for (u32 *p = im.pixels; p != end; ++p) {
        u32 pixel = *p;
        
        b32 found = false;
        ForSB (colors) {
            if (pixel == *it) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            sb_push(colors, pixel);
            sb_push(color_vectors, rgba_to_v3(pixel));
        }
    }
    
    color_palette result = {0};
    result.count = sb_count(colors);
    result.colors = colors;
    result.color_vectors = color_vectors;
    
    printf("palette color count = %zu\n", result.count);
    
    return result;
}

static float color_distance_squared(v3 a, v3 b) {
    // NOTE: from https://www.compuphase.com/cmetric.htm
    
    float mean_r = (a.r + b.r)*0.5f;
    float dr = a.r - b.r;
    float dg = a.g - b.g;
    float db = a.b - b.b;
    
    return ((2.0f + mean_r*(1.0f/256.0f))*square(dr) +
            (4.0f*square(dg)) +
            (2.0f + ((255.0f- mean_r)*(1.0f/256.0f)))*square(db));
}

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s [color palette image file] [input file] [output filename]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *color_palette_filename = argv[1];
    const char *input_filename  = argv[2];
    const char *output_filename = argv[3];
    
    color_palette cp = load_color_palette(color_palette_filename);
    
    int width, height, channel_count;
    u32 *pixels = (u32 *)stbi_load(input_filename, &width, &height, &channel_count, 4);
    
    if (!pixels) {
        fprintf(stderr, "error loading image\n");
        return EXIT_FAILURE;
    }
    
    size_t pixel_count = width*height;
    
    u32 *end = pixels + pixel_count;
    
    u32 *output_pixels = (u32 *)malloc(pixel_count*sizeof(u32));
    
    static u32 memo[1<<24] = {0};
    
    u32 *src = pixels;
    u32 *dest = output_pixels;
    while (src != end) {
        u32 pixel = *src++;
        
        int index = (pixel & 0x00ffffff);
        u32 *table_ptr = &memo[index];
        u32 output_pixel = *table_ptr;
        
        if (!output_pixel) {
            v3 in = rgba_to_v3(pixel);
            
            float min_distance = FLT_MAX;
            size_t index_of_closest_color = 0;
            ForI (cp.count) {
                float dist = color_distance_squared(in, cp.color_vectors[i]);
                if (dist < min_distance) {
                    min_distance = dist;
                    index_of_closest_color = i;
                }
            }
            
            output_pixel = cp.colors[index_of_closest_color];
            *table_ptr = output_pixel;
        }
        
        *dest++ = (pixel & 0xff000000) | (output_pixel & 0x00ffffff);
    }
    
    printf("conversion finished\noutputting png...\n");
    
    stbi_write_png(output_filename, width, height, 4, output_pixels, 0);
    puts("finished.");
}