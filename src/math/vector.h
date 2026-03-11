#ifndef OVERTURE_VECTORS
#define OVERTURE_VECTORS

#include "math/types.h"
#include <float.h>
#include <math.h>

/************
 * Vector 2 *
 ************/

static inline vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
};

static inline vec2_t vec2_subtract(vec2_t a, vec2_t b) {
    vec2_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
};

static inline vec2_t vec2_scale(vec2_t v, float n) {
    vec2_t result;
    result.x = n * v.x;
    result.y = n * v.y;
    return result;
};

static inline float vec2_dot(vec2_t a, vec2_t b){
    return a.x * b.x + a.y * b.y;
};

static inline float vec2_length(vec2_t v) {
    return sqrtf(vec2_dot(v, v));
};

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

static inline vec3_t vec3(float x, float y, float z) {
    return (vec3_t) {
        x, y, z
    };
}

static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

static inline vec3_t vec3_subtract(vec3_t a, vec3_t b) {
    vec3_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

static inline vec3_t vec3_scale(vec3_t v, float n) {
    vec3_t result;
    result.x = n * v.x;
    result.y = n * v.y;
    result.z = n * v.z;
    return result;
}

static inline float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float vec3_length(vec3_t v) {
    return sqrtf(vec3_dot(v, v));
}

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

static inline vec4_t vec4_add(vec4_t a, vec4_t b) {
    vec4_t result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

static inline vec4_t vec4_subtract(vec4_t a, vec4_t b) {
    vec4_t result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

static inline vec4_t vec4_scale(vec4_t v, float n) {
    vec4_t result;

    result.x = n * v.x;
    result.y = n * v.y;
    result.z = n * v.z;
    result.w = n * v.w;

    return result;
}

static inline float vec4_dot(vec4_t a, vec4_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline float vec4_length(vec4_t v) {
    return sqrtf(vec4_dot(v, v));
}

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

#endif
