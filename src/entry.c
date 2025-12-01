#include "core/log.h"
#include "core/systems.h"

//temp
int should_exit = 0;

int main() {
    INFO("Project: %s %d.%d.%d.", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    INFO("Engine: %s %d.%d.%d.", ENGINE_NAME, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);

    TRACE("Test trace.");
    DEBUG("Test debug.");
    INFO("Test info.");
    WARN("Test warning.");
    ERROR("Test error.");
    FATAL("Test fatal.");

    run_systems(SETUP);
    
    while (!should_exit) {
        run_systems(PRE_UPDATE);

        run_systems(UPDATE);

        run_systems(POST_UPDATE);

        run_systems(RENDER);
    }

    run_systems(CLEANUP);
}
