#include <GLFW/glfw3.h>

#include "platform/window.h"
#include "core/log.h"
#include "core/ecs.h"
#include "core/systems.h"
#include "graphics/vulkan.h"

static void error_callback(int error, const char* description) {
    ERROR("GLFW error %d: %s.", error, description);
}

void init_windowing() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        FATAL("Failed to initialize glfw.");
    }

    TRACE("Initialized glfw.");
}

void cleanup_windowing() {
    glfwTerminate();

    TRACE("Cleaned up glfw.");
}

REGISTER_COMPONENT(window_t);

// maybe instead of creating a window component maybe create an entity with window comp?
window_t create_window() {
    window_t window;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window.window = glfwCreateWindow(680, 480, "TEST GAME", NULL, NULL);
    if (!window.window) {
        FATAL("Could not create window.");
    }

    setup_vulkan_window(&window);

    return window;
}

void cleanup_windows() {
    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));

        cleanup_vulkan_window(window);

        glfwDestroyWindow(window->window);
        
        ent_ptr++;
    }

    free(list);
    TRACE("Destroyed windows.");
}

REGISTER_SYSTEM(cleanup_windows, CLEANUP);

void start_window_render() {
    glfwPollEvents();

    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));

        begin_vulkan_window_render(window);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(start_window_render, PRE_RENDER);

void display_to_windows() {
    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));

        end_vulkan_window_render(window);

        vulkan_display_to_window(window);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(display_to_windows, POST_RENDER);
