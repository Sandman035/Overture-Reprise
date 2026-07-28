#include "overture/graphics.h"
#include <overture/overture.h>

#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

float distance = 10;

void setup() {
    entity_t window_ent = create_entity();
    window_t window;
    window.id = create_window();
    ADD_COMPONENT(window_t, window_ent, &window);

    glsl_shader_loader_args_t args;
    args.type = VERTEX_SHADER;
    asset_handle_t vertex_shader = load_asset("assets/shaders/texture.vs", "overture:glsl_shader", &args);

    args.type = FRAGMENT_SHADER;
    asset_handle_t fragment_shader = load_asset("assets/shaders/texture.fs", "overture:glsl_shader", &args);

    asset_handle_t mesh_handle = load_asset("assets/models/backpack.obj", "temp:tinyobj", NULL);

    asset_handle_t texture1 = load_asset("assets/textures/diffuse.jpg", "overture:stb_image", NULL);
    asset_handle_t texture2 = load_asset("assets/textures/ao.jpg", "overture:stb_image", NULL);

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

    // And here is the problem with components with arrays, well dynamic arrays
    // might need to implement a custom allocator to keep track of mem
    // definetely should have a func "add_texture" to streamline process
    textures_t textures;
    textures.count = 2;
    textures.names = malloc(2 * sizeof(char[30]));
    strcpy(textures.names[0], "diffuse");
    strcpy(textures.names[1], "ao");
    textures.textures = malloc(2 * sizeof(asset_handle_t));
    textures.textures[0] = texture1;
    textures.textures[1] = texture2;

    ADD_COMPONENT(render_object_t, model_ent, &model);
    ADD_COMPONENT(z_pre_pass_t, model_ent, &z_pre);
    ADD_COMPONENT(transform_t, model_ent, &transform);
    ADD_COMPONENT(textures_t, model_ent, &textures);

    entity_t camera_ent = create_entity();
    camera_t camera;
    camera.window_id = window.id;
    camera.fov = 45.0f;
    camera.aspect_ratio = 16.0/9.0;
    camera.near = 0.01;
    camera.far = 10000;

    transform_t camera_transform;
    camera_transform.scale = vec3(1, 1, 1);
    camera_transform.pos = vec3(0, 0, distance);
    camera_transform.rot = vec3(0, 0, 0);

    ADD_COMPONENT(camera_t, camera_ent, &camera);
    ADD_COMPONENT(transform_t, camera_ent, &camera_transform);
}

REGISTER_SYSTEM(setup, SETUP);

void update_camera() {
    entity_t* list = FILTER_ENTITIES(camera_t, transform_t);

    if (key_pressed(GLFW_KEY_W)) {
        DEBUG("FORWARD!");
        distance -= 0.5;
    }
    if (key_pressed(GLFW_KEY_S)) {
        DEBUG("BACKWARD!");
        distance += 0.5;
    }
    if (key_just_pressed(GLFW_KEY_SPACE)) {
        distance = 10;
    }

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        transform_t* transform = get_comp(list[i], GET_ID(transform_t));

        transform->pos = vec3(distance * sinf(glfwGetTime()), 0, distance * cosf(glfwGetTime()));

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
