#ifndef OVERTURE_RENDER_OBJ
#define OVERTURE_RENDER_OBJ

#include "platform/window.h"
#include "vulkan_utils.h"

typedef struct {
    window_t* window;
    pipeline_t pipeline;
    buffer_t vertex_buffer;
    buffer_t index_buffer;
} opaque_render_object_t;

typedef struct {
    window_t* window;
    pipeline_t* pipeline;
    buffer_t vertex_buffer;
    buffer_t index_buffer;
} transparent_render_object_t;

#endif
