#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "platform/window.h"
#include "graphics/vulkan.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    window_t* window;
    pipeline_t pipeline;
    buffer_t vertex_buffer;
} triangle_t;

REGISTER_COMPONENT(triangle_t);

typedef struct {
    float pos[2];
    float color[3];
} vertex_t;

const vertex_t vertices[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
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

    triangle_t triangle;
    triangle.window = get_comp(win_ent, GET_ID(window_t));
    create_pipeline(&window, &triangle.pipeline, "res/shaders/vert.spv", "res/shaders/frag.spv", binding);

    create_vertex_buffer(&window, &triangle.vertex_buffer, (void *)vertices, sizeof(vertices));

    add_triangle_t(tri_ent, &triangle);
}

REGISTER_SYSTEM(setup_triangle, SETUP);

void render_triangle() {
    entity_t** list = FILTER_ENTITIES(triangle_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        triangle_t* triangle = get_comp(*ent_ptr, GET_ID(triangle_t));

        bind_graphics_pipeline(triangle->window, triangle->pipeline);
        TRACE("Bound pipeline.");
        bind_vertex_buffer(triangle->window, triangle->vertex_buffer, 0);
        TRACE("Bound vertex buffers.");
        /* draw verticies */
        vkCmdDraw(triangle->window->vulkan_info.command_buffers[triangle->window->vulkan_info.current_frame], 3, 1, 0, 0);
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
        if (should_window_close(window)) {
            should_exit = 1;
        }
        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update, UPDATE);

void cleanup_triangle() {
    entity_t** list = FILTER_ENTITIES(triangle_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        triangle_t* triangle = get_comp(*ent_ptr, GET_ID(triangle_t));

        destroy_vertex_buffer(triangle->window, &triangle->vertex_buffer);
        destroy_pipeline(triangle->window, &triangle->pipeline);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(cleanup_triangle, CLEANUP);
