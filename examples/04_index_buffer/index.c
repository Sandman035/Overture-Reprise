#include <overture/overture.h>

#include "core/ecs.h"
#include "core/systems.h"

typedef struct {
    vec3_t pos;
    vec3_t color;
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

void setup_rect() {
    entity_t* win_ent = create_entity();

    uint32_t window_id = create_window();

    window_t win_comp = { window_id };

    extern void add_window_t_cpy(entity_t*, void*);
    add_window_t_cpy(win_ent, &win_comp);

    entity_t* rect_ent = create_entity();

    opaque_render_object_t rect;
    rect.window_id = window_id;

    rect.color_program = create_program();
    add_shader(rect.color_program, vertex_shader_source, VERTEX_SHADER);
    add_shader(rect.color_program, fragment_shader_source, FRAGMENT_SHADER);

    rect.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
    add_index_buffer(&rect.vertex_buffer, sizeof(indices), (void*)indices);
    add_attrib(&rect.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, pos));
    add_attrib(&rect.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, color));

    ADD_COMPONENT_CPY(opaque_render_object_t, rect_ent, &rect);
}

REGISTER_SYSTEM(setup_rect, SETUP);

extern int should_exit;

void update() {
    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));
        if (should_window_close(window->id)) {
            should_exit = 1;
        }
        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update, UPDATE);
