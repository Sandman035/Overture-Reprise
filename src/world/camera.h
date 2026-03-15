#ifndef OVERTURE_CAMERA
#define OVERTURE_CAMERA

#include <stdint.h>

typedef struct camera_t {
    uint64_t window_id;
    float fov;
    float aspect_ratio;
    float near;
    float far;
} camera_t;

#endif
