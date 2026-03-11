#ifndef OVERTURE_MATH_TYPES
#define OVERTURE_MATH_TYPES

#include <stdint.h>

typedef union vec2_t {
    float raw[2];

    struct {
        float x;
        float y;
    };

    struct {
        float u;
        float v;
    };
} vec2_t;

typedef union vec3_t {
    float raw[3];

    struct {
        float x;
        float y;
        float z;
    };

    struct {
        float u;
        float v;
        float w;
    };

    struct {
        float r;
        float g;
        float b;
    };

    struct {
        float pitch;
        float yaw;
        float roll;
    };
} vec3_t;

typedef union vec4_t {
    float raw[4];

    struct {
        float x;
        float y;
        float z;
        float w;
    };
    
    struct {
        float r;
        float g;
        float b;
        float a;
    };
} vec4_t;

typedef union mat4_t {
    float raw[4][4];

    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
} mat4_t;

#endif
