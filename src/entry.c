#include "core/systems.h"

//temp
int should_exit = 0;

int main() {
    // initialize everything
    init_systems();

    run_systems(SETUP);
    
    while (!should_exit) {
        run_systems(PRE_UPDATE);

        run_systems(UPDATE);

        run_systems(POST_UPDATE);

        run_systems(RENDER);
    }

    run_systems(CLEANUP);

    // cleanup everything else
}
