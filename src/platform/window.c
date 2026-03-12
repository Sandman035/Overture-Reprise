#include "graphics/opengl.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>

#include "platform/window.h"
#include "core/log.h"
#include "core/ecs.h"
#include "core/systems.h"

typedef struct window_node_t {
    uint64_t id;
    window_t window;
    struct window_node_t* next;
} window_node_t;

static window_node_t* window_list_head = NULL;
static window_node_t* window_list_tail = NULL;

static uint64_t current_win_id = 0;

REGISTER_COMPONENT(window_comp_t);

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

uint64_t create_window() {
    window_node_t* node = malloc(sizeof(window_node_t));
    node->next = NULL;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    node->window.window = glfwCreateWindow(680, 480, "TEST GAME", NULL, NULL);
    if (!node->window.window) {
        FATAL("Could not create window.");
    }

    node->id = current_win_id;
    current_win_id++;

    glfwSetFramebufferSizeCallback(node->window.window, framebuffer_size_callback);

    glfwMakeContextCurrent(node->window.window);
    setup_gl_window();

    TRACE("Created new window.");

    if (window_list_head == NULL) {
        window_list_head = node;
        window_list_tail = node;
        return node->id;
    }

    window_list_tail->next = node;
    window_list_tail = node;

    return node->id;
}

window_t* get_window(uint64_t id) {
    window_node_t* temp = window_list_head;
    while (temp != NULL) {
        if (temp->id == id) {
            TRACE("Retrived window %ld.", id);
            return &temp->window;
        }
        temp = temp->next;
    }

    WARN("Window %ld does not exist.", id);
    return NULL;
}

uint32_t should_window_close(window_t* window) {
    return glfwWindowShouldClose(window->window);
}

void cleanup_windows() {
    while (window_list_head != NULL) {
        window_node_t* temp = window_list_head;
        glfwDestroyWindow(temp->window.window);

        window_list_head = temp->next;

        free(temp);
        temp = NULL;
    }

    TRACE("Destroyed windows.");
}

REGISTER_SYSTEM(cleanup_windows, CLEANUP);

void start_window_render() {
    glfwPollEvents();

    window_node_t* temp = window_list_head;
    while (temp != NULL) {
        glfwMakeContextCurrent(temp->window.window);

        // TODO: pass window information such as clear color

        begin_gl_window_render();

        temp = temp->next;
    }
}

REGISTER_SYSTEM(start_window_render, PRE_RENDER);

void display_to_windows() {
    window_node_t* temp = window_list_head;
    while (temp != NULL) {
        glfwMakeContextCurrent(temp->window.window);

        glfwSwapBuffers(temp->window.window);

        temp = temp->next;
    }
}

REGISTER_SYSTEM(display_to_windows, POST_RENDER);
