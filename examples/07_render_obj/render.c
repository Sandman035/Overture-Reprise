#include "core/ecs.h"
#include "core/systems.h"
#include "graphics/opengl.h"
#include "graphics/render_obj.h"
#include "math/types.h"
#include "math/vector.h"
#include "platform/window.h"
#include "world/camera.h"
#include "world/transform.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

// This is a label component store no info but can be used to label entities
typedef struct {} rect_t;

REGISTER_COMPONENT(rect_t);

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
    "uniform mat4 world;\n"
    "uniform mat4 proj;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = proj * world * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragment_shader_source = "#version 430 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\n\0";

void setup_rect() {
    entity_t* win_ent = create_entity();

    uint32_t window_id = create_window();

    window_comp_t win_comp = { window_id };

    ADD_COMPONENT_CPY(window_comp_t, win_ent, &win_comp);

    entity_t* rect_ent = create_entity();

    opaque_render_object_t rect_obj;
    rect_obj.window_id = window_id;

    rect_obj.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
    add_index_buffer(&rect_obj.vertex_buffer, sizeof(indices), (void*)indices);
    add_attrib(&rect_obj.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, pos));
    add_attrib(&rect_obj.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, color));

    rect_obj.color_program = create_program();
    add_shader(rect_obj.color_program, vertex_shader_source, VERTEX_SHADER);
    add_shader(rect_obj.color_program, fragment_shader_source, FRAGMENT_SHADER);

    rect_obj.depth_program = create_program();
    add_shader(rect_obj.depth_program, vertex_shader_source, VERTEX_SHADER);

    ADD_COMPONENT_CPY(opaque_render_object_t, rect_ent, &rect_obj);

    ADD_COMPONENT_EMPTY(rect_t, rect_ent);
    
    transform_t rect_transform; 
    rect_transform.pos = vec3(0, 0, -3);
    rect_transform.scale = vec3(0.5, 0.5, 0.5);
    rect_transform.rot = vec3(0, 0, 0);

    ADD_COMPONENT_CPY(transform_t, rect_ent, &rect_transform);

    entity_t* camera_ent = create_entity();

    camera_t camera;
    camera.window_id = window_id;
    camera.fov = 45.0f;
    camera.aspect_ratio = 16.0/9.0;
    camera.near = 0.01;
    camera.far = 100;

    ADD_COMPONENT_CPY(camera_t, camera_ent, &camera);
}

REGISTER_SYSTEM(setup_rect, SETUP);

void update_rect() {
    entity_t** list = FILTER_ENTITIES(rect_t, transform_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        transform_t* transform = get_comp(*ent_ptr, GET_ID(transform_t));

        transform->pos = vec3(
            -sinf(glfwGetTime() / 7 * 4) * 0.7, 
            sinf(glfwGetTime() / 5.7 * 4) * 0.7, 
            2 * sinf(glfwGetTime()) - 3
        );

        transform->rot = vec3(glfwGetTime(), glfwGetTime(), glfwGetTime());

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update_rect, UPDATE);

void update_camera() {
    entity_t** list = FILTER_ENTITIES(camera_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        camera_t* camera = get_comp(*ent_ptr, GET_ID(camera_t));
        

        int32_t width, height;
        glfwGetWindowSize(get_window(camera->window_id)->window, &width, &height);
        camera->aspect_ratio = ((float)width)/((float)height);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update_camera, UPDATE);

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
