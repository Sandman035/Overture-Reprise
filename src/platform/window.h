#ifndef OVERTURE_WINDOW
#define OVERTURE_WINDOW

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "graphics/vulkan_utils.h"

// rename or smt
void init_windowing();
void cleanup_windowing();

// TODO: window id or smt
typedef struct {
    GLFWwindow* window;
    vulkan_window_info_t vulkan_info;
} window_t;

window_t create_window();

#endif
