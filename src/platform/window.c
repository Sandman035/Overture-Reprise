#include "graphics/opengl.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>

#include "platform/window.h"
#include "core/log.h"
#include "core/ecs.h"
#include "core/systems.h"
#include "graphics/render_obj.h"

typedef struct window_node_t {
    window_data_t window;
    struct window_node_t* next;
} window_node_t;

static window_node_t* window_list_head = NULL;
static window_node_t* window_list_tail = NULL;

static uint64_t current_win_id = 0;

REGISTER_COMPONENT(window_t);

static void error_callback(int error, const char* description) {
    ERROR("GLFW error %d: %s.", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
    window_data_t* user_window = glfwGetWindowUserPointer(window);

    glfwMakeContextCurrent(window);

    resize_gl_viewport(width, height);

    if (user_window != NULL) {
        // temp
        user_window->context.width = width;
        user_window->context.height = height;

        // TODO: resize framebuffers

        TRACE("Window %d resized to %dx%d with aspect ratio of %f.", user_window->id, width, height, (float)width / (float)height);

        return;
    }
    TRACE("Window: %p resized to %dx%d with aspect ratio of %f.", window, width, height, (float)width / (float)height);
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

    // TODO: have a debug define to not include this if not debuging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    node->window.window = glfwCreateWindow(680, 480, "TEST GAME", NULL, NULL);
    if (!node->window.window) {
        FATAL("Could not create window.");
    }

    glfwSetWindowUserPointer(node->window.window, &node->window);

    node->window.id = current_win_id;
    current_win_id++;

    glfwSetFramebufferSizeCallback(node->window.window, framebuffer_size_callback);

    glfwMakeContextCurrent(node->window.window);
    setup_gl_window();

    // NOTE: idk if this needs to be done for every window or not
    setup_gl_window_debug_callback();

    // TODO: configure this properly
    node->window.context = create_object_renderer_context(680, 480);

    TRACE("Created new window.");

    if (window_list_head == NULL) {
        window_list_head = node;
        window_list_tail = node;
        return node->window.id;
    }

    window_list_tail->next = node;
    window_list_tail = node;

    return node->window.id;
}

window_data_t* get_window(uint64_t id) {
    window_node_t* temp = window_list_head;
    while (temp != NULL) {
        if (temp->window.id == id) {
            TRACE("Retrived window %ld.", id);
            return &temp->window;
        }
        temp = temp->next;
    }

    WARN("Window %ld does not exist.", id);
    return NULL;
}

uint32_t should_window_close(uint64_t id) {
    return glfwWindowShouldClose(get_window(id)->window);
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
    glfwPollEvents(); // maybe this should'nt be here but at the start of the game loop

    window_node_t* temp = window_list_head;
    while (temp != NULL) {
        glfwMakeContextCurrent(temp->window.window);

        // temp
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //clear_object_rederer_framebuffers(&temp->window.context);
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
