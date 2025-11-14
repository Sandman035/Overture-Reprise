#ifndef OVERTURE_ECS
#define OVERTURE_ECS

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"

typedef unsigned char* signature_t;
typedef void* component_t;
typedef void** entity_t; // ptr to a void*

// component registration funcs and other tools
unsigned long register_new_comp();
void add_comp(unsigned long ent_id, unsigned long comp_id, void* data, size_t size);

// entity funcs
unsigned long add_ent();
void remove_ent(unsigned long id);
entity_t get_ent(unsigned long id);

// component funcs
int has_comp(unsigned long ent_id, unsigned long comp_id);
component_t get_comp(unsigned long ent_id, unsigned long comp_id);
component_t get_comp_from_ent(entity_t ent, unsigned long comp_id);
void remove_comp(unsigned long ent_id, unsigned long comp_id);

entity_t* filter_entities(signature_t filter);

#define FILTER_ENTITIES(...) ({ \
    signature_t filter = CREATE_SIG(__VA_ARGS__); \
    entity_t* list = filter_entities(filter); \
    free(filter); \
    list; \
})

//tools
signature_t id_to_sig(unsigned long id);
void add_sig(signature_t s1, const signature_t s2);

signature_t create_sig(int n, ...);

#define GET_ID(component_struct) ({extern unsigned long component_struct ## _id; component_struct ## _id;})
#define GET_SIG(component_struct) id_to_sig(GET_ID(component_struct))

#define X_ID(X) GLUE(X,_id)
#define ID_DECLARE(...) extern unsigned long __VA_ARGS__
#define CREATE_SIG(...) ({ \
    EVAL(ID_DECLARE TRANSFORM(X_ID,(__VA_ARGS__))); \
    create_sig(VARCOUNT(__VA_ARGS__), TRANSFORM(X_ID,(__VA_ARGS__))); \
})

// TODO: serialize and deserialize component structs (maybe using X macros)
// https://natecraun.net/articles/struct-iteration-through-abuse-of-the-c-preprocessor.html 
// will need to expand this to include arrays

// using ddlexport on win add_struct_name will be called using dlsym well on linux
#define REGISTER_COMPONENT(struct_name) \
    unsigned long struct_name ## _id = 0; \
    void add_ ## struct_name(unsigned long id, void* data) { \
        if (struct_name ## _id == 0) { \
            struct_name ## _id = register_new_comp(); \
        } \
        add_comp(id, struct_name ## _id, data, sizeof(struct_name)); \
    } \

#endif
