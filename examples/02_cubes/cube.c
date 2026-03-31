#include <overture/overture.h>

#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>

// This is a label component; stores no data but can be used to label entities
typedef struct {} cube_t;

REGISTER_COMPONENT(cube_t);

typedef struct {
    vec3_t pos;
    vec3_t color;
} vertex_t;

const vertex_t vertices[] = {
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},

    {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},

    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
};

const uint32_t indices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
};

const char *vertex_shader_source ="#version 430 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 world;\n"
    "uniform mat4 proj;\n"
    "uniform mat4 view;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = proj * view * world * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragment_shader_source = "#version 430 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\n\0";

void setup_cube() {
    entity_t window_ent = create_entity();
    window_t window;
    window.id = create_window();
    ADD_COMPONENT(window_t, window_ent, &window);

    for (int32_t x = -1; x <= 1; x++) {
        for (int32_t y = -1; y <= 1; y++) {
            for (int32_t z = -1; z <= 1; z++) {
                entity_t cube_ent = create_entity();

                render_object_t cube;

                cube.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
                add_attrib(&cube.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, pos));
                add_attrib(&cube.vertex_buffer, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, color));
                add_index_buffer(&cube.vertex_buffer, sizeof(indices), (void*)indices);

                cube.program = create_program();
                add_shader(cube.program, vertex_shader_source, VERTEX_SHADER);
                add_shader(cube.program, fragment_shader_source, FRAGMENT_SHADER);

                cube.window_id = window.id;

                z_pre_pass_t z_pre;

                z_pre.program = create_program();
                add_shader(z_pre.program, vertex_shader_source, VERTEX_SHADER);

                transform_t cube_tranform;
                cube_tranform.scale = vec3(1, 1, 1);
                cube_tranform.pos = vec3(x * 5, y * 5, z * 5);
                cube_tranform.rot = vec3(0, 0, 0);

                ADD_COMPONENT_EMPTY(cube_t, cube_ent);
                ADD_COMPONENT(render_object_t, cube_ent, &cube);
                ADD_COMPONENT(z_pre_pass_t, cube_ent, &z_pre);
                ADD_COMPONENT(transform_t, cube_ent, &cube_tranform);
            }
        }
    }

    entity_t camera_ent = create_entity();
    camera_t camera;
    camera.window_id = window.id;
    camera.fov = 45.0f;
    camera.aspect_ratio = 16.0/9.0;
    camera.near = 0.01;
    camera.far = 10000;

    transform_t camera_transform;
    camera_transform.scale = vec3(1, 1, 1);
    camera_transform.pos = vec3(0, 0, 30);
    camera_transform.rot = vec3(0, 0, 0);

    ADD_COMPONENT(camera_t, camera_ent, &camera);
    ADD_COMPONENT(transform_t, camera_ent, &camera_transform);
}

REGISTER_SYSTEM(setup_cube, SETUP);

void update_cube() {
    entity_t* list = FILTER_ENTITIES(cube_t, transform_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        transform_t* transform = get_comp(list[i], GET_ID(transform_t));

        transform->rot = vec3(glfwGetTime(), glfwGetTime(), glfwGetTime());
    }

    free(list);
}

REGISTER_SYSTEM(update_cube, UPDATE);

void update_camera() {
    entity_t* list = FILTER_ENTITIES(camera_t, transform_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        transform_t* transform = get_comp(list[i], GET_ID(transform_t));

        transform->pos = vec3(30 * sinf(glfwGetTime()), 0, 30 * cosf(glfwGetTime()));

        transform->rot = vec3(0, glfwGetTime(), 0);
    }

    free(list);
}

REGISTER_SYSTEM(update_camera, UPDATE);

extern int should_exit;
void update() {
    entity_t* list = FILTER_ENTITIES(window_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        window_t* window = get_comp(list[i], GET_ID(window_t));
        if (should_window_close(window->id)) {
            should_exit = 1;
        }
    }

    free(list);
}

REGISTER_SYSTEM(update, UPDATE);
