#ifndef OVERTURE_CORE_H
#define OVERTURE_CORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "private_core.h"
#include "private_macros.h"

/*******
 * LOG *
 *******/

typedef enum log_level_t {
    LEVEL_TRACE,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
    LEVEL_FATAL
} log_level_t;

#ifndef NDEBUG
#define TRACE(...) print_log(LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG(...) print_log(LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) print_log(LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#else
#define TRACE(...)
#define DEBUG(...)
#define INFO(...)
#endif
#define WARN(...) print_log(LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) print_log(LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define FATAL(...) print_log(LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/***********
 * Systems *
 ***********/

typedef void(*system_ptr_t)(void);

typedef enum schedule_t {
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

/// Register system into the engine to be run at the schedule specified.
#define REGISTER_SYSTEM(system, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        register_system(system, schedule); \
    }

/// Register system into the engine to be run at the schedule specified at the front of the queue.
#define REGISTER_SYSTEM_FRONT(system, schedule) \
    __attribute__((constructor)) \
    void add_ ## system() { \
        register_system_front(system, schedule); \
    }

/*******
 * ECS *
 *******/

typedef uint8_t* signature_t;
typedef void* component_t;
typedef uint64_t entity_t;

#define ENTITY_INVALID UINT64_MAX

void add_comp(entity_t ent, uint64_t comp_id, void* data, size_t size);

entity_t create_entity();
void remove_ent(entity_t id);

component_t get_comp(entity_t ent, uint64_t comp_id);
void remove_comp(entity_t ent, uint64_t comp_id);

void cleanup_ecs();

#define FILTER_ENTITIES(...) ({ \
    signature_t filter = CREATE_SIG(__VA_ARGS__); \
    entity_t* list = filter_entities(filter); \
    free(filter); \
    list; \
})

// Takes the signatures as tuples
#define FILTER_ENTITIES_EXCLUDING(include, exclude) ({ \
    signature_t include_filter = CREATE_SIG include; \
    signature_t exclude_filter = CREATE_SIG exclude; \
    entity_t* list = filter_entities_excluding(include_filter, exclude_filter); \
    free(include_filter); \
    free(exclude_filter); \
    list; \
})

#define GET_ID(component_struct) ({extern uint64_t component_struct ## _id; component_struct ## _id;})
#define GET_SIG(component_struct) id_to_sig(GET_ID(component_struct))

#define CREATE_SIG(...) ({ \
    extern uint64_t MAP_LIST(X_ID,__VA_ARGS__); \
    MAP(REGISTER_ID,__VA_ARGS__)\
    create_sig(VARCOUNT(__VA_ARGS__), MAP_LIST(X_ID,__VA_ARGS__)); \
})

// using ddlexport on win add_struct_name will be called using dlsym etc
#define REGISTER_COMPONENT(struct_name) \
    uint64_t struct_name ## _id = 0; \
    void add_ ## struct_name(entity_t ent, void* data) { \
        if (struct_name ## _id == 0) { \
            struct_name ## _id = register_new_comp(sizeof(struct_name)); \
            TRACE("Registered id %d for %s", struct_name ## _id, #struct_name); \
        } \
        add_comp(ent, struct_name ## _id, data, sizeof(struct_name)); \
    }

#define ADD_COMPONENT(struct_name, entity, data) \
    extern void add_ ## struct_name (entity_t, void*); \
    add_ ## struct_name (entity, data);

#define ADD_COMPONENT_EMPTY(struct_name, entity) \
    extern void add_ ## struct_name (entity_t, void*); \
    add_ ## struct_name (entity, NULL);

#endif
