#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/vulkan.h"
#include "graphics/vulkan_utils.h"
#include "platform/window.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    window_t* window;
    pipeline_t pipeline;
    buffer_t vertex_buffer;
    buffer_t index_buffer;
} rect_t;

REGISTER_COMPONENT(rect_t);

typedef struct {
    float pos[2];
    float color[3];
} vertex_t;

const vertex_t vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const uint16_t indices[] = {
    0, 1, 2, 2, 3, 0
};

void setup_triangle() {
    entity_t* win_ent = create_entity();

    window_t window = create_window();

    extern void add_window_t(entity_t*, void*);
    add_window_t(win_ent, &window);

    entity_t* tri_ent = create_entity();

    vertex_binding_t binding;
    binding.binding_description = (VkVertexInputBindingDescription) {0, sizeof(vertex_t), VK_VERTEX_INPUT_RATE_VERTEX};
    binding.attribute_count = 2;
    VkVertexInputAttributeDescription attrib_desc[] = {
        {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_t, pos)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex_t, color)}
    };
    binding.attribute_description = attrib_desc;

    rect_t rect;
    rect.window = get_comp(win_ent, GET_ID(window_t));
    create_pipeline(&window, &rect.pipeline, "res/shaders/vert.spv", "res/shaders/frag.spv", binding);

    create_vertex_buffer(&window, &rect.vertex_buffer, (void *)vertices, sizeof(vertices));
    create_index_buffer(&window, &rect.index_buffer, (void *)indices, sizeof(indices));

    add_rect_t(tri_ent, &rect);
}

REGISTER_SYSTEM(setup_triangle, SETUP);

void render_triangle() {
    entity_t** list = FILTER_ENTITIES(rect_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        rect_t* rect = get_comp(*ent_ptr, GET_ID(rect_t));
        /* bind pipeline */
        vkCmdBindPipeline(rect->window->vulkan_info.command_buffers[rect->window->vulkan_info.current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, rect->pipeline.pipeline);
        TRACE("Bound pipeline.");
        /* bind vertex and index buffer */
        VkBuffer vertexBuffers[] = {rect->vertex_buffer.buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(rect->window->vulkan_info.command_buffers[rect->window->vulkan_info.current_frame], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(rect->window->vulkan_info.command_buffers[rect->window->vulkan_info.current_frame], rect->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        TRACE("Bound vertex and index buffers.");
        /* draw verticies */
        vkCmdDrawIndexed(rect->window->vulkan_info.command_buffers[rect->window->vulkan_info.current_frame], 6, 1, 0, 0, 0);
        TRACE("Drawn triangles.");

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(render_triangle, RENDER);

extern int should_exit;

void update() {
    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));
        if (glfwWindowShouldClose(window->window)) {
            should_exit = 1;
        }
        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update, UPDATE);

void cleanup_triangle() {
    entity_t** list = FILTER_ENTITIES(rect_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        rect_t* rect = get_comp(*ent_ptr, GET_ID(rect_t));

        destroy_vertex_buffer(rect->window, &rect->vertex_buffer);
        destroy_index_buffer(rect->window, &rect->index_buffer);
        destroy_pipeline(rect->window, &rect->pipeline);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(cleanup_triangle, CLEANUP);
