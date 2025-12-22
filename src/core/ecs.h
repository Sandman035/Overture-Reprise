#ifndef OVERTURE_ECS
#define OVERTURE_ECS

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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
typedef component_t* entity_t; // ptr to a void*

// component registration funcs and other tools
uint64_t register_new_comp();
void add_comp(uint64_t ent_id, uint64_t comp_id, void* data, size_t size);

// entity funcs
uint64_t add_ent();
void remove_ent(uint64_t id);
entity_t get_ent(uint64_t id);

// component funcs
int has_comp(uint64_t ent_id, uint64_t comp_id);
component_t get_comp(uint64_t ent_id, uint64_t comp_id);
component_t get_comp_from_ent(entity_t ent, uint64_t comp_id);
void remove_comp(uint64_t ent_id, uint64_t comp_id);

entity_t* filter_entities(signature_t filter);

#define FILTER_ENTITIES(...) ({ \
    signature_t filter = CREATE_SIG(__VA_ARGS__); \
    entity_t* list = filter_entities(filter); \
    free(filter); \
    list; \
})

signature_t id_to_sig(uint64_t id);

signature_t create_sig(uint32_t n, ...);

#define GET_ID(component_struct) ({extern uint64_t component_struct ## _id; component_struct ## _id;})
#define GET_SIG(component_struct) id_to_sig(GET_ID(component_struct))

#define X_ID(X) X ## _id
#define CREATE_SIG(...) ({ \
    extern uint64_t MAP_LIST(X_ID,__VA_ARGS__); \
    create_sig(VARCOUNT(__VA_ARGS__), MAP_LIST(X_ID,__VA_ARGS__)); \
})

// using ddlexport on win add_struct_name will be called using dlsym well on linux
#define REGISTER_COMPONENT(struct_name) \
    uint64_t struct_name ## _id = 0; \
    void add_ ## struct_name(uint64_t id, void* data) { \
        if (struct_name ## _id == 0) { \
            struct_name ## _id = register_new_comp(); \
        } \
        add_comp(id, struct_name ## _id, data, sizeof(struct_name)); \
    } \

#endif
