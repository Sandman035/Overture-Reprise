#include "graphics/opengl.h"
#include <GLFW/glfw3.h>
#include <stdint.h>

#include "platform/window.h"
#include "core/log.h"
#include "core/ecs.h"
#include "core/systems.h"

static void error_callback(int error, const char* description) {
    ERROR("GLFW error %d: %s.", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
    TRACE("Window: %p resized to %dx%d.", window, width, height);

    glfwMakeContextCurrent(window);

    resize_gl_viewport(width, height);
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

// TODO: free window memory once window is closed
window_t* create_window() {
    window_t* window = malloc(sizeof(window_t));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window->window = glfwCreateWindow(680, 480, "TEST GAME", NULL, NULL);
    if (!window->window) {
        FATAL("Could not create window.");
    }

    TRACE("Created new window.");

    glfwSetFramebufferSizeCallback(window->window, framebuffer_size_callback);

    glfwMakeContextCurrent(window->window);
    setup_gl_window();

    return window;
}

uint32_t should_window_close(window_t* window) {
    return glfwWindowShouldClose(window->window);
}

void cleanup_windows() {
    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));

        glfwDestroyWindow(window->window);
        
        ent_ptr++;
    }

    free(list);
    TRACE("Destroyed windows.");
}

REGISTER_SYSTEM(cleanup_windows, POST_CLEANUP);

void start_window_render() {
    glfwPollEvents();

    entity_t** list = FILTER_ENTITIES(window_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        window_t* window = get_comp(*ent_ptr, GET_ID(window_t));

        glfwMakeContextCurrent(window->window);

        // TODO: pass window information such as clear color

        begin_gl_window_render();

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

        glfwMakeContextCurrent(window->window); // TODO: might not be needed here but during rendering idk

        glfwSwapBuffers(window->window);

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(display_to_windows, POST_RENDER);
