#ifndef OVERTURE_RENDER_OBJ
#define OVERTURE_RENDER_OBJ

#include "graphics/opengl.h"
#include "math/types.h"
#include <stdint.h>

typedef struct {
    uint32_t depth_fbo;
    uint32_t depth_rbo;
    uint32_t opaque_fbo;
    uint32_t opaque_color_texture;
    uint32_t transparent_fbo;
    uint32_t transparent_color_texture;
    uint32_t width, height;
    mat4_t view;
    mat4_t proj;
} object_renderer_context;

object_renderer_context create_object_renderer_context(uint32_t width, uint32_t height);

void clear_object_rederer_framebuffers(object_renderer_context* context);

typedef struct {
    uint64_t window_id;
    program_t color_program;
    program_t depth_program;
    vertex_buffer_t vertex_buffer;
    mat4_t world_transform;
} opaque_render_object_t;

typedef struct {
    uint64_t window_id;
    program_t program;
    vertex_buffer_t vertex_buffer;
    mat4_t world_transform;
} transparent_render_object_t;

#endif
