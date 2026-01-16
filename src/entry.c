#include "core/systems.h"
#include "graphics/opengl.h"
#include "platform/window.h"

//temp
int should_exit = 0;

int main() {
    init_windowing();

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

    cleanup_opengl();
    cleanup_windowing();
}
