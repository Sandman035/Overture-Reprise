#ifndef OVERTURE_PLATFORM_H
#define OVERTURE_PLATFORM_H

#include <stdint.h>

/// Window component structure.
typedef struct window_t {
    uint64_t id;
} window_t;

/// Creates window data and returns window id.
uint64_t create_window();

/// Returns wether a window has been signaled to close.
uint32_t should_window_close(uint64_t id);

#endif
