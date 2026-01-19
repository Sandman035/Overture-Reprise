#ifndef OVERTURE_ECS
#define OVERTURE_ECS

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "core/log.h"
#include "macros.h"

/*
 * TODO: maybe have an entity struct that contains both the component list and signature then
 *       every time either an ent_id or an entity_t is need this new struct can be passed
 *       streamlining the process
 *
 *       this requires a full rework of the ecs
 */

typedef uint8_t* signature_t;
typedef void* component_t;

typedef struct {
    uint64_t id;
    component_t* components;
    signature_t signature;
} entity_t;

// component registration funcs and other tools
uint64_t register_new_comp();
void add_comp_cpy(entity_t* ent, uint64_t comp_id, void* data, size_t size);
void add_comp_store(entity_t* ent, uint64_t comp_id, void* data, size_t size);

// entity funcs
entity_t* create_entity();
void remove_ent(uint64_t id);
entity_t* get_ent(uint64_t id);

// component funcs
component_t get_comp(entity_t* ent, uint64_t comp_id);
void remove_comp(entity_t* ent, uint64_t comp_id);

entity_t** filter_entities(signature_t filter);

#define FILTER_ENTITIES(...) ({ \
    signature_t filter = CREATE_SIG(__VA_ARGS__); \
    entity_t** list = filter_entities(filter); \
    free(filter); \
    list; \
})

signature_t id_to_sig(uint64_t id);

signature_t create_sig(uint32_t n, ...);

#define GET_ID(component_struct) ({extern uint64_t component_struct ## _id; component_struct ## _id;})
#define GET_SIG(component_struct) id_to_sig(GET_ID(component_struct))

#define X_ID(X) X ## _id
#define REGISTER_ID(X) \
    if (X ## _id == 0) { \
        X ## _id = register_new_comp(); \
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

#endif
