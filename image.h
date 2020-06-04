#ifndef IMAGE_H
#define IMAGE_H

typedef struct {
    u32 width;
    u32 height;
    u32 *pixels;
} image;

static image load_image(const char *filename) {
    image result = {0};
    
    int w, h;
    result.pixels = (u32 *)stbi_load(filename, &w, &h,  0, 4);
    
    if (!result.pixels) return result;
    
    result.width = w;
    result.height = h;
    
    return result;
}

static v3 rgba_to_v3(u32 p) {
    u8 r = p;
    u8 g = p >> 8;
    u8 b = p >> 16;
    return (v3){(float)r, (float)g, (float)b};
}

u32 v3_to_rgba(v3 v) {
    u8 r = round_to_u32(v.r);
    u8 g = round_to_u32(v.g);
    u8 b = round_to_u32(v.b);
    
    return (0xff << 24) | (b << 16) | (g << 8) | r;
}

#endif //IMAGE_H
