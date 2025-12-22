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
} triangle_t;

REGISTER_COMPONENT(triangle_t);

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
    unsigned long win_ent = add_ent();

    window_t window = create_window();

    extern void add_window_t(unsigned long, void*);
    add_window_t(win_ent, &window);

    unsigned long tri_ent = add_ent();

    vertex_binding_t binding;
    binding.binding_description = (VkVertexInputBindingDescription) {0, sizeof(vertex_t), VK_VERTEX_INPUT_RATE_VERTEX};
    binding.attribute_count = 2;
    VkVertexInputAttributeDescription attrib_desc[] = {
        {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_t, pos)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex_t, color)}
    };
    binding.attribute_description = attrib_desc;

    triangle_t triangle;
    triangle.window = get_comp(win_ent, GET_ID(window_t));
    create_pipeline(&window, &triangle.pipeline, "res/shaders/vert.spv", "res/shaders/frag.spv", binding);

    create_vertex_buffer(&window, &triangle.vertex_buffer, (void *)vertices, sizeof(vertices));
    create_index_buffer(&window, &triangle.index_buffer, (void *)indices, sizeof(indices));

    add_triangle_t(tri_ent, &triangle);
}

REGISTER_SYSTEM(setup_triangle, SETUP);

void render_triangle() {
    entity_t* list = FILTER_ENTITIES(triangle_t);

    entity_t* ent = list;
    while (*ent != NULL) {
        triangle_t* triangle = get_comp_from_ent(*ent, GET_ID(triangle_t));
        /* bind pipeline */
        vkCmdBindPipeline(triangle->window->vulkan_info.command_buffers[triangle->window->vulkan_info.current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, triangle->pipeline.pipeline);
        TRACE("Bound pipeline.");
        /* bind vertex and index buffer */
        VkBuffer vertexBuffers[] = {triangle->vertex_buffer.buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(triangle->window->vulkan_info.command_buffers[triangle->window->vulkan_info.current_frame], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(triangle->window->vulkan_info.command_buffers[triangle->window->vulkan_info.current_frame], triangle->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        TRACE("Bound vertex and index buffers.");
        /* draw verticies */
        vkCmdDrawIndexed(triangle->window->vulkan_info.command_buffers[triangle->window->vulkan_info.current_frame], 6, 1, 0, 0, 0);
        TRACE("Drawn triangles.");

        ent++;
    }

    free(list);
}

REGISTER_SYSTEM(render_triangle, RENDER);

extern int should_exit;

void update() {
    entity_t* list = FILTER_ENTITIES(window_t);

    entity_t* ent = list;
    while (*ent != NULL) {
        window_t* window = get_comp_from_ent(*ent, GET_ID(window_t));
        if (glfwWindowShouldClose(window->window)) {
            should_exit = 1;
        }
        ent++;
    }

    free(list);
}

REGISTER_SYSTEM(update, UPDATE);

void cleanup_triangle() {
    entity_t* list = FILTER_ENTITIES(triangle_t);

    entity_t* ent = list;
    while (*ent != NULL) {
        triangle_t* triangle = get_comp_from_ent(*ent, GET_ID(triangle_t));

        destroy_vertex_buffer(triangle->window, &triangle->vertex_buffer);
        destroy_index_buffer(triangle->window, &triangle->index_buffer);
        destroy_pipeline(triangle->window, &triangle->pipeline);

        ent++;
    }

    free(list);
}

REGISTER_SYSTEM(cleanup_triangle, CLEANUP);
