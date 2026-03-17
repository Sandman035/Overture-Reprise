#ifndef OVERTURE_RENDER_OBJ
#define OVERTURE_RENDER_OBJ

#include "graphics/opengl.h"
#include "math/types.h"
#include <stdint.h>

typedef struct {
    uint32_t opaque_fbo;
    uint32_t opaque_color_texture;
    uint32_t opaque_depth_rbo;
    uint32_t transparent_fbo;
    uint32_t transparent_color_texture;
    uint32_t width, height;
    mat4_t view;
    mat4_t proj;
} object_renderer_context;

object_renderer_context create_object_renderer_context(uint32_t width, uint32_t height);

void rebuild_object_renderer_framebuffers(object_renderer_context* context, uint32_t width, uint32_t height);

void create_object_renderer_framebuffers(object_renderer_context* context, uint32_t width, uint32_t height);

void destroy_object_renderer_framebuffers(object_renderer_context* context);

void clear_object_rederer_framebuffers(object_renderer_context* context);

typedef struct {
    uint64_t window_id;
    program_t program;
    vertex_buffer_t vertex_buffer;
    mat4_t world_transform;
} render_object_t;

typedef struct {
    program_t program;
} z_pre_pass_t;

typedef struct {} transparent_material_t;

#endif
