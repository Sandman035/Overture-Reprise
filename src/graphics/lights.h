#ifndef OVERTURE_LIGHTS
#define OVERTURE_LIGHTS

#include "math/types.h"

typedef struct {
    vec3_t colour;
    float intensity;
    uint64_t window_id;
} point_light_t;

typedef struct {
    vec3_t pos;
    float _pad0;
    vec3_t colour;
    float intensity;
} point_light_info_t;

#endif
