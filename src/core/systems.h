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

#define REGISTER_SYSTEM(system, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        register_system(system, schedule); \
    }

// doesn't work don't use
#define REGISTER_SYSTEM_BEFORE(system, target, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        register_system_before(system, target, schedule); \
    }

#define REGISTER_SYSTEM_FRONT(system, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        register_system_front(system, schedule); \
    }

void register_system(system_ptr_t system, schedule_t schedule);
void register_system_front(system_ptr_t system, schedule_t schedule);
// doesn't work don't use
void register_system_before(system_ptr_t system, system_ptr_t target, schedule_t schedule);

void run_systems_sequential(schedule_t schedule);
void run_systems_parrallel(schedule_t schedule);

#endif
