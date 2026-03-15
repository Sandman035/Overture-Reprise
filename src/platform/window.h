#ifndef OVERTURE_WINDOW
#define OVERTURE_WINDOW

#include "graphics/render_obj.h"
#include <stdint.h>
#include <GLFW/glfw3.h>


// rename or smt
void init_windowing();
void cleanup_windowing();

// temp
typedef struct window_t {
    uint64_t id;
} window_t;

typedef struct {
    uint64_t id;
    GLFWwindow* window;
    object_renderer_context context;
} window_data_t;

uint64_t create_window();

window_data_t* get_window(uint64_t id);

uint32_t should_window_close(uint64_t id);

#endif
