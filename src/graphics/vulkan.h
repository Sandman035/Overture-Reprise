#ifndef OVERTURE_VULKAN
#define OVERTURE_VULKAN

#include "graphics/vulkan_utils.h"
#include "platform/window.h"
#include <stdint.h>
#include <vulkan/vulkan.h>

void setup_vulkan();

void setup_vulkan_window(window_t* window);
void begin_vulkan_window_render(window_t* window);
void end_vulkan_window_render(window_t* window);
void vulkan_display_to_window(window_t* window);
void cleanup_vulkan_window(window_t* window);

void cleanup_vulkan();

void create_pipeline(window_t* window, pipeline_t* pipeline,  const char* vertex_shader, const char* fragment_shader, vertex_binding_t vertex_binding);
void destroy_pipeline(window_t* window, pipeline_t* pipeline);

void create_vertex_buffer(window_t* window, vertex_buffer_t* vertex_buffer, void* verticies, size_t size);
void destroy_vertex_buffer(window_t* window, vertex_buffer_t* vertex_buffer);

#endif
