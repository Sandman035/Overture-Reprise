#ifndef OVERTURE_RENDER_OBJ
#define OVERTURE_RENDER_OBJ

#include "assets/asset_manager.h"
#include "core/ecs.h"
#include "graphics/opengl.h"
#include "math/types.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t n;
    entity_t* entities;
    uint32_t allocated;
} render_queue_t;

typedef struct {
    uint32_t opaque_fbo;
    uint32_t opaque_color_texture;
    uint32_t opaque_depth_rbo;
    uint32_t transparent_fbo;
    uint32_t transparent_color_texture;
    uint32_t width, height;
    mat4_t view;
    mat4_t proj;
    render_queue_t opaque_z_pre;
    render_queue_t opaque_no_z;
    render_queue_t transparent;
} object_renderer_context;

object_renderer_context create_object_renderer_context(uint32_t width, uint32_t height);

void rebuild_object_renderer_framebuffers(object_renderer_context* context, uint32_t width, uint32_t height);

void create_object_renderer_framebuffers(object_renderer_context* context, uint32_t width, uint32_t height);

void destroy_object_renderer_framebuffers(object_renderer_context* context);

void clear_object_rederer_framebuffers(object_renderer_context* context);

void clear_render_queues(object_renderer_context* context);

void render_queues(object_renderer_context* context);

typedef struct {
    uint64_t window_id;
    program_t program;
    asset_handle_t mesh;
    mat4_t world_transform;
} render_object_t;

typedef struct {
    program_t program;
} z_pre_pass_t;

typedef struct {} transparent_material_t;

#endif
