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
} vertex_custom_t;

const vertex_custom_t vertices[] = {
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

void setup_cube() {
    for (int i = 0; i < 3; i++) {
        entity_t window_ent = create_entity();
        window_t window;
        window.id = create_window();
        ADD_COMPONENT(window_t, window_ent, &window);

        // due to my windowing system shaders must be created after the window is initiallized
        glsl_shader_loader_args_t args;
        args.type = VERTEX_SHADER;
        asset_handle_t vertex_shader = load_asset("assets/shaders/cube.vs", "overture:glsl_shader", &args);

        args.type = FRAGMENT_SHADER;
        asset_handle_t fragment_shader = load_asset("assets/shaders/cube.fs", "overture:glsl_shader", &args);

        entity_t cube_ent = create_entity();
 
        render_object_t cube;
 
        mesh_asset_t mesh;

        mesh.vertex_buffer = create_vertex_buffer(sizeof(vertices), (void*)vertices);
        instert_attrib(&mesh.vertex_buffer, 0, 3, GL_FLOAT, sizeof(vertex_custom_t), offsetof(vertex_custom_t, pos));
        instert_attrib(&mesh.vertex_buffer, 1, 3, GL_FLOAT, sizeof(vertex_custom_t), offsetof(vertex_custom_t, color));
        add_index_buffer(&mesh.vertex_buffer, sizeof(indices), (void*)indices);

        cube.mesh = add_asset(&mesh, sizeof(mesh_asset_t), "mesh"); // the type is currently not used
 
        cube.program = create_program();
        add_shader_asset(cube.program, vertex_shader);
        add_shader_asset(cube.program, fragment_shader);
 
        cube.window_id = window.id;
 
        z_pre_pass_t z_pre;
 
        z_pre.program = create_program();
        add_shader_asset(z_pre.program, vertex_shader);

        transform_t cube_tranform;
        cube_tranform.scale = vec3(1, 1, 1);
        cube_tranform.pos = vec3(0, 0, 0);
        cube_tranform.rot = vec3(0, 0, 0);
 
        ADD_COMPONENT_EMPTY(cube_t, cube_ent);
        ADD_COMPONENT(render_object_t, cube_ent, &cube);
        ADD_COMPONENT(z_pre_pass_t, cube_ent, &z_pre);
        ADD_COMPONENT(transform_t, cube_ent, &cube_tranform);

        entity_t camera_ent = create_entity();
        camera_t camera;
        camera.window_id = window.id;
        camera.fov = 45.0f;
        camera.aspect_ratio = 16.0/9.0;
        camera.near = 0.01;
        camera.far = 10000;

        transform_t camera_transform;
        camera_transform.scale = vec3(1, 1, 1);
        camera_transform.pos = vec3(0, 0, 5);
        camera_transform.rot = vec3(0, 0, 0);

        ADD_COMPONENT(camera_t, camera_ent, &camera);
        ADD_COMPONENT(transform_t, camera_ent, &camera_transform);
    }
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
