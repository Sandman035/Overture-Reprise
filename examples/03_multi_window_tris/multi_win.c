#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/opengl.h"
#include "platform/window.h"
#include <GLFW/glfw3.h>

typedef struct {
    window_t* window;
    program_t program;
    vertex_buffer_t vertex_buffer;
} triangle_t;

REGISTER_COMPONENT(triangle_t);

typedef struct {
    float pos[3];
    float color[3];
} vertex_t;

const vertex_t vertices[] = {
    {{0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f,-0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

const vertex_t vertices2[] = {
    {{0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}},
    {{-0.5f,-0.5f, 0.0f}, {0.0f, 1.0f, 1.0f}}
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

void setup_multi_win() {
    for (int i = 0; i < 3; i++) {
        entity_t* win_ent = create_entity();

        window_t* window = create_window();

        extern void add_window_t_store(entity_t*, void*);
        add_window_t_store(win_ent, window);

        entity_t* tri_ent = create_entity();

        triangle_t triangle;
        triangle.window = get_comp(win_ent, GET_ID(window_t));

        triangle.program = create_program();
        add_shader(triangle.program, vertex_shader_source, VERTEX_SHADER);
        add_shader(triangle.program, fragment_shader_source, FRAGMENT_SHADER);

        if (i % 2 != 0) {
            triangle.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
        } else {
            triangle.vertex_buffer = create_vertex_buffer(sizeof(vertices2), (void*)vertices2);
        }

        add_attrib(&triangle.vertex_buffer, 3, GL_FLOAT, 6 * sizeof(float), offsetof(vertex_t, pos));
        add_attrib(&triangle.vertex_buffer, 3, GL_FLOAT, 6 * sizeof(float), offsetof(vertex_t, color));

        add_triangle_t_cpy(tri_ent, &triangle);
    }
}

REGISTER_SYSTEM(setup_multi_win, SETUP);

void render_triangle() {
    entity_t** list = FILTER_ENTITIES(triangle_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        triangle_t* triangle = get_comp(*ent_ptr, GET_ID(triangle_t));

        glfwMakeContextCurrent(triangle->window->window);

        glUseProgram(triangle->program);

        glBindVertexArray(triangle->vertex_buffer.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        TRACE("Draw triangle.");

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

        destroy_vertex_buffer(&triangle->vertex_buffer);
        destroy_program(triangle->program);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(cleanup_triangle, CLEANUP);
