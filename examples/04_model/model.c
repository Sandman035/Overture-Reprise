#include <overture/overture.h>

#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>

void setup() {
    entity_t window_ent = create_entity();
    window_t window;
    window.id = create_window();
    ADD_COMPONENT(window_t, window_ent, &window);

    glsl_shader_loader_args_t args;
    args.type = VERTEX_SHADER;
    asset_handle_t vertex_shader = load_asset("assets/shaders/diffuse.vs", "overture:glsl_shader", &args);

    args.type = FRAGMENT_SHADER;
    asset_handle_t fragment_shader = load_asset("assets/shaders/diffuse.fs", "overture:glsl_shader", &args);

    asset_handle_t mesh_handle = load_asset("assets/models/bunny.obj", "temp:tinyobj", &args);

    entity_t model_ent = create_entity();

    render_object_t model;

    model.mesh = mesh_handle;

    model.program = create_program();
    add_shader_asset(model.program, vertex_shader);
    add_shader_asset(model.program, fragment_shader);

    model.window_id = window.id;

    z_pre_pass_t z_pre;

    z_pre.program = create_program();
    add_shader_asset(z_pre.program, vertex_shader);

    transform_t transform;
    transform.scale = vec3(1, 1, 1);
    transform.pos = vec3(0, 0, 0);
    transform.rot = vec3(0, 0, 0);

    ADD_COMPONENT(render_object_t, model_ent, &model);
    ADD_COMPONENT(z_pre_pass_t, model_ent, &z_pre);
    ADD_COMPONENT(transform_t, model_ent, &transform);

    entity_t camera_ent = create_entity();
    camera_t camera;
    camera.window_id = window.id;
    camera.fov = 45.0f;
    camera.aspect_ratio = 16.0/9.0;
    camera.near = 0.01;
    camera.far = 10000;

    transform_t camera_transform;
    camera_transform.scale = vec3(1, 1, 1);
    camera_transform.pos = vec3(0, 0.1, 0.5);
    camera_transform.rot = vec3(0, 0, 0);

    ADD_COMPONENT(camera_t, camera_ent, &camera);
    ADD_COMPONENT(transform_t, camera_ent, &camera_transform);
}

REGISTER_SYSTEM(setup, SETUP);

void update_camera() {
    entity_t* list = FILTER_ENTITIES(camera_t, transform_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        transform_t* transform = get_comp(list[i], GET_ID(transform_t));

        transform->pos = vec3(0.5 * sinf(glfwGetTime()), 0.1, 0.5 * cosf(glfwGetTime()));

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
