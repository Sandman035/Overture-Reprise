#ifndef OVERTURE_MATH_H
#define OVERTURE_MATH_H

#include <stdint.h>
#include <float.h>
#include <math.h>

/// Vector in R2.
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

/// Vector R3.
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

/// Vector in R4.
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

/// 4x4 matrix.
typedef union mat4_t {
    float raw[4][4];

    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
} mat4_t;

/************
 * Vector 2 *
 ************/

/// Add two vectors in R2.
static inline vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
};

/// Subtract two vectors in R2.
static inline vec2_t vec2_subtract(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
};

/// Scale a vector in R2 by n.
static inline vec2_t vec2_scale(vec2_t v, float n) {
    vec2_t result;
    result.x = n * v.x;
    result.y = n * v.y;
    return result;
};

/// The dot product of two vectors in R2.
static inline float vec2_dot(vec2_t a, vec2_t b){
    return a.x * b.x + a.y * b.y;
};

/// Return the length of a vector in R2.
static inline float vec2_length(vec2_t v) {
    return sqrtf(vec2_dot(v, v));
};

/// Normalize a vector in R2.
static inline vec2_t vec2_normalize(vec2_t v){
    vec2_t result;
    result.x = 0;
    result.y = 0;

    float length = vec2_length(v);

    if (length > FLT_EPSILON) {
        result.x = v.x / length;
        result.y = v.y / length;
    }

    return result;
};

/************
 * Vector 3 *
 ************/

/// Create a vector in R3.
static inline vec3_t vec3(float x, float y, float z) {
    return (vec3_t) {
        x, y, z
    };
}

/// Add two vectors in R3.
static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

/// Subtract two vectors in R3.
static inline vec3_t vec3_subtract(vec3_t a, vec3_t b) {
    vec3_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

/// Scale a vector in R3 by n.
static inline vec3_t vec3_scale(vec3_t v, float n) {
    vec3_t result;
    result.x = n * v.x;
    result.y = n * v.y;
    result.z = n * v.z;
    return result;
}

/// The dot product of two vectors in R3.
static inline float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/// The length of a vector in R3.
static inline float vec3_length(vec3_t v) {
    return sqrtf(vec3_dot(v, v));
}

/// Normalize a vector in R3.
static inline vec3_t vec3_normalize(vec3_t v) {
    vec3_t result;
    result.x = 0;
    result.y = 0;
    result.z = 0;

    float length = vec3_length(v);

    if (length > FLT_EPSILON) {
        result.x = v.x / length;
        result.y = v.y / length;
        result.z = v.z / length;
    }

    return result;
}

/// The cross product of a vector in R3.
static inline vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t result;

    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;

    return result;
}

/************
 * Vector 4 *
 ************/

/// Add two vectors in R4.
static inline vec4_t vec4_add(vec4_t a, vec4_t b) {
    vec4_t result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

/// Subtract two vectors in R4.
static inline vec4_t vec4_subtract(vec4_t a, vec4_t b) {
    vec4_t result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

/// Scale a vector in R4 by n.
static inline vec4_t vec4_scale(vec4_t v, float n) {
    vec4_t result;

    result.x = n * v.x;
    result.y = n * v.y;
    result.z = n * v.z;
    result.w = n * v.w;

    return result;
}

/// The dot product of two vectors in R4.
static inline float vec4_dot(vec4_t a, vec4_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/// The length of a vector in R4.
static inline float vec4_length(vec4_t v) {
    return sqrtf(vec4_dot(v, v));
}

/// Normalize a vector in R4.
static inline vec4_t vec4_normalize(vec4_t v) {
    vec4_t result;
    result.x = 0;
    result.y = 0;
    result.z = 0;
    result.w = 0;

    float length = vec4_length(v);

    if (length > FLT_EPSILON) {
        result.x = v.x / length;
        result.y = v.y / length;
        result.z = v.z / length;
        result.w = v.w / length;
    }

    return result;
}

/*********
 * Mat 4 *
 *********/

/// 4x4 identity matrix.
static inline mat4_t mat4_identity() {
    return (mat4_t) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

/// 4x4 translation matrix.
static inline mat4_t mat4_translation(vec3_t v) {
    return (mat4_t) {
        1, 0, 0, v.x,
        0, 1, 0, v.y,
        0, 0, 1, v.z,
        0, 0, 0, 1
    };
}

/// 4x4 rotation matrix.
///
/// Rotations are along the X, Y, Z axes
static inline mat4_t mat4_rotation(vec3_t v) {
    return (mat4_t) {
        cosf(v.roll) * cosf(v.yaw), cosf(v.roll) * sinf(v.yaw) * sinf(v.pitch) - sinf(v.roll) * cosf(v.pitch), cosf(v.roll) * sinf(v.yaw) * cosf(v.pitch) + sinf(v.roll) * sinf(v.pitch), 0,
        sinf(v.roll) * cosf(v.yaw), sinf(v.roll) * sinf(v.yaw) * sinf(v.pitch) + cosf(v.roll) * cosf(v.pitch), sinf(v.roll) * sinf(v.yaw) * cosf(v.pitch) - cosf(v.roll) * sinf(v.pitch), 0,
        -sinf(v.yaw),               cosf(v.yaw) * sinf(v.pitch),                                                cosf(v.yaw) * cosf(v.pitch),                                               0,
        0,                          0,                                                                          0,                                                                          1
    };
}

/// 4x4 scaling matrix.
static inline mat4_t mat4_scaling(vec3_t v) {
    return (mat4_t) {
        v.x, 0,   0,   0,
        0,   v.y, 0,   0,
        0,   0,   v.z, 0,
        0,   0,   0,   1
    };
}

/// Multiply two 4x4 matrices.
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

/// Translate a 4x4 matrix by a vector in R3.
static inline mat4_t translate_mat4(mat4_t m, vec3_t v) {
    return mult_mat4(mat4_translation(v), m);
}

/// Rotate a 4x4 matrix by a vector in R3.
static inline mat4_t rotate_mat4(mat4_t m, vec3_t v) {
    return mult_mat4(mat4_rotation(v), m);
}

/// Scale a 4x4 matrix by a vector in R3.
static inline mat4_t scale_mat4(mat4_t m, vec3_t v) {
    return mult_mat4(mat4_scaling(v), m);
}

/// Tranform a vector in R4 by a 4x4 matrix.
static inline vec4_t transform_vec4_mat4(vec4_t v, mat4_t m) {
    return (vec4_t) {
        v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + v.w * m.m03,
        v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + v.w * m.m13,
        v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + v.w * m.m23,
        v.x * m.m30 + v.y * m.m31 + v.z * m.m32 + v.w * m.m33,
    };
}

/// Tranform a vector in R3 by a 4x4 matrix.
static inline vec3_t transform_vec3_mat4(vec3_t v, mat4_t m) {
    return (vec3_t) {
        v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + 1 * m.m03,
        v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + 1 * m.m13,
        v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + 1 * m.m23,
    };
}

/// 4x4 perspective projection matrix using vertical fov, aspect ration width/height.
static inline mat4_t mat4_perspective_proj(float fov, float aspect_ratio, float near, float far) {
    float t = tanf((fov / 2) * (M_PI / 180));
    float top = near * t;
    float r = top * aspect_ratio;

    return (mat4_t) {
        near/r, 0, 0, 0,
        0, near/top, 0, 0,
        0, 0, -((far + near) / (far - near)), -((2 * far * near)/(far - near)),
        0, 0, -1, 0
    };
}

#endif
