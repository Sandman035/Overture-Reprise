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

typedef struct entity_t {
    uint64_t id;
    component_t* components;
    signature_t signature;
} entity_t;

typedef entity_t entity_t;

/// Create a new entity.
entity_t* create_entity();

/// Remove entity by id.
void remove_ent(uint64_t id);

/// Get entity by id.
entity_t* get_ent(uint64_t id);

/// Get component from entity by id.
component_t get_comp(entity_t* ent, uint64_t comp_id);

/// Remove component from entity by id.
void remove_comp(entity_t* ent, uint64_t comp_id);

/// Filter entities that contain components listed.
#define FILTER_ENTITIES(...) ({ \
    signature_t filter = CREATE_SIG(__VA_ARGS__); \
    entity_t** list = filter_entities(filter); \
    free(filter); \
    list; \
})

/// Get the id of a component by name;
#define GET_ID(component_struct) ({extern uint64_t component_struct ## _id; component_struct ## _id;})

/// Get the signature of a component by name;
#define GET_SIG(component_struct) id_to_sig(GET_ID(component_struct))

/// Create a signature from the entities listed.
#define CREATE_SIG(...) ({ \
    extern uint64_t MAP_LIST(X_ID,__VA_ARGS__); \
    MAP(REGISTER_ID,__VA_ARGS__)\
    create_sig(VARCOUNT(__VA_ARGS__), MAP_LIST(X_ID,__VA_ARGS__)); \
})

/// Register a component structure to be used with the ECS.
#define REGISTER_COMPONENT(struct_name) \
    uint64_t struct_name ## _id = 0; \
    void add_ ## struct_name ## _cpy(entity_t* ent, void* data) { \
        if (struct_name ## _id == 0) { \
            struct_name ## _id = register_new_comp(); \
            TRACE("Registered id %d for %s", struct_name ## _id, #struct_name); \
        } \
        add_comp_cpy(ent, struct_name ## _id, data, sizeof(struct_name)); \
    } \
    void add_ ## struct_name ## _store(entity_t* ent, void* data) { \
        if (struct_name ## _id == 0) { \
            struct_name ## _id = register_new_comp(); \
            TRACE("Registered id %d for %s", struct_name ## _id, #struct_name); \
        } \
        add_comp_store(ent, struct_name ## _id, data, sizeof(struct_name)); \
    } \

/// Add a component to an entity copying the data.
#define ADD_COMPONENT_CPY(struct_name, entity, data) \
    extern void add_ ## struct_name ## _cpy(entity_t*, void*); \
    add_ ## struct_name ## _cpy(entity, data);

/// Add a component to an entity storing the pointer to the data.
#define ADD_COMPONENT_STORE(struct_name, entity, data) \
    extern void add_ ## struct_name ## _store(entity_t*, void*); \
    add_ ## struct_name ## _store(entity, data);

/// Add an empty component to an entity.
#define ADD_COMPONENT_EMPTY(struct_name, entity) \
    extern void add_ ## struct_name ## _store(entity_t*, void*); \
    add_ ## struct_name ## _store(entity, NULL);

#endif
