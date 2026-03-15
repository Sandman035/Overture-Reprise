#ifndef OVERTURE_WINDOW
#define OVERTURE_WINDOW

#include "graphics/render_obj.h"
#include <stdint.h>
#include <GLFW/glfw3.h>


// NOTE: maybe window entities or window components should be stored in a list of some sorts
//       with an id, then a get window funcion would fetch the window or error if window doesn't exist
//       right now windows are reffered to by a pointer to the window struct which can lead
//       to problems if the window gets destroyed

// rename or smt
void init_windowing();
void cleanup_windowing();

// temp
typedef struct window_comp_t {
    uint64_t id;
} window_comp_t;

typedef struct window_t {
    GLFWwindow* window;
    object_renderer_context context;
} window_t;

uint64_t create_window();

window_t* get_window(uint64_t id);

uint32_t should_window_close(window_t* window);

#endif
