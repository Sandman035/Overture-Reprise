#include <overture/overture.h>

#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

float distance = 10;

void setup() {
    load_asset("assets/maps/texture.otm", "overture:text_map", NULL);
}

REGISTER_SYSTEM(setup, SETUP);

void update_camera() {
    entity_t* list = FILTER_ENTITIES(camera_t, transform_t);

    if (key_pressed(GLFW_KEY_W)) {
        distance -= 0.5;
    }
    if (key_pressed(GLFW_KEY_S)) {
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
