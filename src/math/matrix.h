#ifndef OVERTURE_MATRIX
#define OVERTURE_MATRIX

#include "math/types.h"
#include <math.h>
#include <stdio.h>

static void print_mat4(mat4_t m) {
    printf("%f, %f, %f, %f \n%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f\n",
        m.m00, m.m01, m.m02, m.m03,
        m.m10, m.m11, m.m12, m.m13,
        m.m20, m.m21, m.m22, m.m23,
        m.m30, m.m31, m.m32, m.m33
    );
}

static inline mat4_t mat4_identity() {
    return (mat4_t) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

static inline mat4_t mat4_translation(vec3_t v) {
    return (mat4_t) {
        1, 0, 0, v.x,
        0, 1, 0, v.y,
        0, 0, 1, v.z,
        0, 0, 0, 1
    };
}

// I have no idea if this is right, need to check
// problem comes from -Z being forward instead of -X
// should be correct unless typo
// (https://en.wikipedia.org/wiki/Rotation_matrix#General_3D_rotations)
static inline mat4_t mat4_rotation(vec3_t v) {
    return (mat4_t) {
        cosf(v.roll) * cosf(v.yaw), cosf(v.roll) * sinf(v.yaw) * sinf(v.pitch) - sinf(v.roll) * cosf(v.pitch), cosf(v.roll) * sinf(v.yaw) * cosf(v.pitch) + sinf(v.roll) * sinf(v.pitch), 0,
        sinf(v.roll) * cosf(v.yaw), sinf(v.roll) * sinf(v.yaw) * sinf(v.pitch) + cosf(v.roll) * cosf(v.pitch), sinf(v.roll) * sinf(v.yaw) * cosf(v.pitch) + cosf(v.roll) * sinf(v.pitch), 0,
        -sin(v.yaw),                cosf(v.yaw) * sinf(v.pitch),                                            cosf(v.yaw) * cosf(v.pitch),                                                  0,
        0,                          0,                                                                       0,                                                                           1
    };
}

static inline mat4_t mat4_scaling(vec3_t v) {
    return (mat4_t) {
        v.x, 0,   0,   0,
        0,   v.y, 0,   0,
        0,   0,   v.z, 0,
        0,   0,   0,   1
    };
}

static inline mat4_t mult_mat4(mat4_t a, mat4_t b) {
    return (mat4_t) {
        a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30, 
        a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31,
        a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32, 
        a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33,

        a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30, 
        a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31, 
        a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32, 
        a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33,

        a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30, 
        a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31, 
        a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32, 
        a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33,

        a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30, 
        a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31, 
        a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32, 
        a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33
    };
}

// these three mat4 transformations can be optimized later
static inline mat4_t translate_mat4(mat4_t m, vec3_t v) {
    return mult_mat4(mat4_translation(v), m);
}

static inline mat4_t rotate_mat4(mat4_t m, vec3_t v) {
    return mult_mat4(mat4_rotation(v), m);
}

static inline mat4_t scale_mat4(mat4_t m, vec3_t v) {
    return mult_mat4(mat4_scaling(v), m);
}

static inline vec4_t transform_vec4_mat4(vec4_t v, mat4_t m) {
    return (vec4_t) {
        v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + v.w * m.m03,
        v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + v.w * m.m13,
        v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + v.w * m.m23,
        v.x * m.m30 + v.y * m.m31 + v.z * m.m32 + v.w * m.m33,
    };
}

// technically this is still a vec4 transformation but the 4th component is always assumed to be 1
static inline vec3_t transform_vec3_mat4(vec3_t v, mat4_t m) {
    return (vec3_t) {
        v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + 1 * m.m03,
        v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + 1 * m.m13,
        v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + 1 * m.m23,
    };
}

static inline mat4_t mat4_perspective_proj(float fov, float aspect_ratio, float near, float far) {
    float s = 1 / tanf((fov / 2) * (M_PI / 180));

    return (mat4_t) {
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, -(far / (far - near)), -1,
        0, 0, -((far * near) / (far - near)), 9
    };
}

#endif
