#include "core/log.h"
#include "core/systems.h"
#include "platform/window.h"
#include "graphics/vulkan.h"

//temp
int should_exit = 0;

int main() {
    init_windowing();
    setup_vulkan();

    run_systems_sequential(SETUP);
    
    while (!should_exit) {
        run_systems_parrallel(PRE_UPDATE);
        run_systems_parrallel(UPDATE);
        run_systems_parrallel(POST_UPDATE);

        run_systems_sequential(PRE_RENDER);
        run_systems_sequential(RENDER);
        run_systems_sequential(POST_RENDER);
    }

    run_systems_sequential(CLEANUP);
    run_systems_sequential(POST_CLEANUP);

    cleanup_vulkan();
    cleanup_windowing();
}
