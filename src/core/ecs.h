#ifndef OVERTURE_ECS
#define OVERTURE_ECS

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "core/log.h"
#include "macros.h"

typedef uint8_t* signature_t;
typedef void* component_t;
typedef uint64_t entity_t;

#define ENTITY_INVALID UINT64_MAX

// component registration funcs and other tools
uint64_t register_new_comp(size_t size);
void add_comp(entity_t ent, uint64_t comp_id, void* data);

// entity funcs
entity_t create_entity();
void remove_ent(entity_t id);

// component funcs
component_t get_comp(entity_t ent, uint64_t comp_id);
void remove_comp(entity_t ent, uint64_t comp_id);

entity_t* filter_entities(signature_t filter);
entity_t* filter_entities_excluding(signature_t include, signature_t exclude);

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

signature_t id_to_sig(uint64_t id);

signature_t create_sig(uint32_t n, ...);

#define GET_ID(component_struct) ({extern uint64_t component_struct ## _id; component_struct ## _id;})
#define GET_SIG(component_struct) id_to_sig(GET_ID(component_struct))

#define X_ID(X) X ## _id
#define REGISTER_ID(X) \
    if (X ## _id == 0) { \
        X ## _id = register_new_comp(sizeof(X)); \
        TRACE("Registered id %d for %s", X ## _id, #X); \
    }
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
        add_comp(ent, struct_name ## _id, data); \
    }

#define ADD_COMPONENT(struct_name, entity, data) \
    extern void add_ ## struct_name (entity_t, void*); \
    add_ ## struct_name (entity, data);

#define ADD_COMPONENT_EMPTY(struct_name, entity) \
    extern void add_ ## struct_name (entity_t, void*); \
    add_ ## struct_name (entity, NULL);

#endif
