#ifndef OVERTURE_SHADER_ASSET
#define OVERTURE_SHADER_ASSET

#include "graphics/opengl.h"

typedef struct {
    shader_type_t type;
    uint32_t shader_id;
} shader_asset_t;

typedef struct {
    shader_type_t type;
} glsl_shader_loader_args_t;

#endif
