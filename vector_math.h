
#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <xmmintrin.h>

static inline s32 round_to_s32(float f) {
    return (s32)roundf(f);
}

static inline u32 round_to_u32(float f) {
    // assert(f >= 0.0f);
    return (u32)(f + 0.5f);
}

static inline u32 ceil_to_u32(float f) {
    return (u32)ceilf(f);
}

static inline float fraction_part(float f) {
    return f - (s32)(f);
}

static inline float float_abs(float f) {
    // UB type cast
    
    u32 u = *(u32 *)&f & 0x7fffffff;
    return *(float *)&u;
}

static inline float square(float x) {
    return x*x;
}

static inline float square_root(float x) {
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
}

typedef union {
    struct {
        float x, y, z;
    };
    struct {
        float r, g, b;
    };
} v3;

static inline v3 v3_scalar_mult(float a, v3 v) {
    return (v3){a*v.x, a*v.y, a*v.z};
}

static inline v3 v3_add(v3 a, v3 b) {
    return (v3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline float dot(v3 a, v3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline v3 cross(v3 a, v3 b) {
    return (v3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

static inline float distance_squared(v3 a, v3 b) {
    return square(a.r - b.r) + square(a.g - b.g) + square(a.b - b.b);
}

static inline float distance(v3 a, v3 b) {
    return square_root(distance_squared(a, b));
}

static inline v3 lerp(v3 a, float t, v3 b) {
    return v3_add(v3_scalar_mult(1.0f - t, a), v3_scalar_mult(t, b));
}

#endif // VECTOR_MATH_H