#ifndef OVERTURE_VULKAN
#define OVERTURE_VULKAN

#include "graphics/vulkan_utils.h"
#include "platform/window.h"
#include <stdint.h>
#include <vulkan/vulkan.h>

void setup_vulkan();
void cleanup_vulkan();

void setup_vulkan_window(window_t* window);
void begin_vulkan_window_render(window_t* window);
void end_vulkan_window_render(window_t* window);
void vulkan_display_to_window(window_t* window);
void cleanup_vulkan_window(window_t* window);

void recreate_swapchain(window_t* window);
void destroy_swapchain(window_t* window);

// include descriptor set layout
void create_pipeline(window_t* window, pipeline_t* pipeline,  const char* vertex_shader, const char* fragment_shader, vertex_binding_t vertex_binding);
void destroy_pipeline(window_t* window, pipeline_t* pipeline);
void bind_graphics_pipeline(window_t* window, pipeline_t pipeline);

void create_vertex_buffer(window_t* window, buffer_t* vertex_buffer, void* verticies, size_t size);
void destroy_vertex_buffer(window_t* window, buffer_t* vertex_buffer);
void bind_vertex_buffer(window_t* window, buffer_t vertex_buffer, const uint64_t offset);

void create_index_buffer(window_t* window, buffer_t* index_buffer, void* indicies, size_t size);
void destroy_index_buffer(window_t* window, buffer_t* index_buffer);
void bind_index_buffer(window_t* window, buffer_t index_buffer, uint64_t offset, VkIndexType index_type);

void create_uniform_buffer();
void destroy_uniform_buffer();

#endif
