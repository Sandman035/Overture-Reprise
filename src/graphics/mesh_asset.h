#ifndef OVERTURE_MESH_ASSET
#define OVERTURE_MESH_ASSET

#include "graphics/opengl.h"
#include "math/types.h"

typedef struct {
    vec3_t pos;
    vec3_t norm;
    vec2_t tex;
} vertex_t;

typedef struct {
    vertex_buffer_t vertex_buffer;
} mesh_asset_t;

#endif
