#ifndef OVERTURE_WORLD_H
#define OVERTURE_WORLD_H

#include <overture/math.h>
#include <stdint.h>

/// World transform component.
typedef struct transform_t {
    vec3_t pos;
    vec3_t rot;
    vec3_t scale;
} transform_t;

/// Camera component.
typedef struct camera_t {
    uint64_t window_id;
    float fov;
    float aspect_ratio;
    float near;
    float far;
} camera_t;

#endif
