#ifndef OVERTURE_SYSTEMS
#define OVERTURE_SYSTEMS

typedef void(*system_ptr_t)(void);

typedef enum {
    SETUP,
    PRE_UPDATE,
    UPDATE,
    POST_UPDATE,
    PRE_RENDER,
    RENDER,
    POST_RENDER,
    CLEANUP,
    NUM_OF_SCHEDULES
} schedule_t;

// maybe register system with a filter and then filtering can be done by the engine instead of user maybe idk
#define REGISTER_SYSTEM(system, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        register_system(system, schedule); \
    }

void register_system(system_ptr_t system, schedule_t schedule);
void run_systems_sequential(schedule_t schedule);
void run_systems_parrallel(schedule_t schedule);

#endif
