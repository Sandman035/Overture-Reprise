#ifndef OVERTURE_WINDOW
#define OVERTURE_WINDOW

#include <stdint.h>
#include <GLFW/glfw3.h>

// rename or smt
void init_windowing();
void cleanup_windowing();

// TODO: window id or smt
typedef struct {
    GLFWwindow* window;
} window_t;

window_t* create_window();

uint32_t should_window_close(window_t* window);

#endif
