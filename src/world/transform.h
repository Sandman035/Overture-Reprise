#ifndef OVERTURE_TRANSFORM
#define OVERTURE_TRANSFORM

#include "math/types.h"

typedef struct transform_t {
    vec3_t pos;
    vec3_t rot;
    vec3_t scale;
} transform_t;

#endif
