#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/opengl.h"
#include "platform/window.h"
#include <GLFW/glfw3.h>
#include <stdint.h>

typedef struct {
    window_t* window;
    program_t program;
    vertex_buffer_t vertex_buffer;
} rect_t;

REGISTER_COMPONENT(rect_t);

typedef struct {
    float pos[3];
    float color[3];
} vertex_t;

const vertex_t vertices[] = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};

const uint32_t indices[] = {
    0, 1, 2, 2, 3, 0
};

const char *vertex_shader_source ="#version 430 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragment_shader_source = "#version 430 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

void setup_triangle() {
    entity_t* win_ent = create_entity();

    window_t* window = create_window();

    extern void add_window_t_store(entity_t*, void*);
    add_window_t_store(win_ent, window);

    entity_t* rect_ent = create_entity();

    rect_t rect;
    rect.window = get_comp(win_ent, GET_ID(window_t));

    rect.program = create_program();
    add_shader(rect.program, vertex_shader_source, VERTEX_SHADER);
    add_shader(rect.program, fragment_shader_source, FRAGMENT_SHADER);

    rect.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
    add_index_buffer(&rect.vertex_buffer, sizeof(indices), (void*)indices);
    add_attrib(&rect.vertex_buffer, 3, GL_FLOAT, 6 * sizeof(float), offsetof(vertex_t, pos));
    add_attrib(&rect.vertex_buffer, 3, GL_FLOAT, 6 * sizeof(float), offsetof(vertex_t, color));

    add_rect_t_cpy(rect_ent, &rect);
}

REGISTER_SYSTEM(setup_triangle, SETUP);

void render_triangle() {
    entity_t** list = FILTER_ENTITIES(rect_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        rect_t* rect = get_comp(*ent_ptr, GET_ID(rect_t));

        glfwMakeContextCurrent(rect->window->window);

        glUseProgram(rect->program);

        glBindVertexArray(rect->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        TRACE("Draw rectangle.");

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
    entity_t** list = FILTER_ENTITIES(rect_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        rect_t* rect = get_comp(*ent_ptr, GET_ID(rect_t));

        destroy_vertex_buffer(&rect->vertex_buffer);
        destroy_program(rect->program);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(cleanup_triangle, CLEANUP);
