#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/opengl.h"
#include "math/types.h"
#include "platform/window.h"
#include <GLFW/glfw3.h>
#include <stddef.h>

typedef struct {
    uint64_t window_id;
    program_t program;
    vertex_buffer_t vertex_buffer;
} triangle_t;

REGISTER_COMPONENT(triangle_t);

typedef struct {
    vec3_t pos;
    vec3_t color;
} vertex_t;

const vertex_t vertices[] = {
    {{0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f,-0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
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
    "uniform vec4 newColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = newColor * vec4(ourColor, 1.0);;\n"
    "}\n\0";

void setup_triangle() {
    entity_t* win_ent = create_entity();

    uint32_t window_id = create_window();

    window_comp_t win_comp = { window_id };

    extern void add_window_comp_t_cpy(entity_t*, void*);
    add_window_comp_t_cpy(win_ent, &win_comp);

    entity_t* tri_ent = create_entity();

    triangle_t triangle;
    triangle.window_id = window_id;

    triangle.program = create_program();
    add_shader(triangle.program, vertex_shader_source, VERTEX_SHADER);
    add_shader(triangle.program, fragment_shader_source, FRAGMENT_SHADER);

    triangle.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
    add_attrib(&triangle.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, pos));
    add_attrib(&triangle.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, color));

    add_triangle_t_cpy(tri_ent, &triangle);
}

REGISTER_SYSTEM(setup_triangle, SETUP);

void render_triangle() {
    entity_t** list = FILTER_ENTITIES(triangle_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        triangle_t* triangle = get_comp(*ent_ptr, GET_ID(triangle_t));

        glfwMakeContextCurrent(get_window(triangle->window_id)->window);

        glUseProgram(triangle->program);

        SET_UNIFORM(4f, triangle->program, "newColor", 0.2f, 0.2f, 0.2f, 1.0f);

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
    entity_t** list = FILTER_ENTITIES(window_comp_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_comp_t* window = get_comp(*ent_ptr, GET_ID(window_comp_t));
        if (should_window_close(get_window(window->id))) {
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
