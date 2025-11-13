#ifndef OVERTURE_SYSTEMS
#define OVERTURE_SYSTEMS

typedef void(*system_ptr_t)(void);

typedef enum {
    SETUP,
    PRE_UPDATE,
    UPDATE,
    POST_UPDATE,
    RENDER,
    CLEANUP,
    NUM_OF_SCHEDULES
} schedule_t;

// maybe register system with a filter and then filtering can be done by the engine instead of user maybe idk
#define REGISTER_SYSTEM(system, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        add_system(system, schedule); \
    }

void init_systems();

void register_system(system_ptr_t system, schedule_t schedule);
void run_systems(schedule_t schedule);

void add_system(system_ptr_t system, schedule_t schedule);

#endif
