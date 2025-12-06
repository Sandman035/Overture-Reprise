#ifndef SERDE_OVERTURE
#define SERDE_OVERTURE

#include "macros.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// inspired by https://natecraun.net/articles/struct-iteration-through-abuse-of-the-c-preprocessor.html

// While there is a macro to generate this structure this can be done manually (like if u have an array in ur struct)
struct struct_fmt {
    char const *s_name; //technically i don't need this too but could be usefull
    size_t num_members;
    size_t struct_size;
    size_t packed_size;
    size_t *offsets;
    size_t *sizes;
    //names? not necessaru but could be usefull
};

// https://stackoverflow.com/questions/58279658/c-variadic-macro-for-pairs
#define DECLARE_VAR(type, name) type name;
#define DECLARE_VAR_PAIR(pair) DECLARE_VAR pair
#define SIZE_OF_TYPE_ADD(type, name) sizeof(type) +
#define SIZE_OF_TYPE_ADD_PAIR(pair) SIZE_OF_TYPE_ADD pair
#define GET_SIZE_OF(type, name) sizeof(type)
#define GET_SIZE_OF_PAIR(pair) GET_SIZE_OF pair
#define GET_VAR_NAME(type, name) name
#define GET_OFFSET(pair, name) offsetof(name, GET_VAR_NAME pair)

// the global fmt variables will also be accessed using dlopen

/// CREATE_SERIALIZABLE_STRUCT(struct name, (type, var_name), (type, var_name), ...)
/// accepts only simple types (no pointers, no arrays) for now
#define CREATE_SERIALIZABLE_STRUCT(struct_name,...) \
    typedef struct { \
        MAP(DECLARE_VAR_PAIR, __VA_ARGS__) \
    } struct_name; \
    \
    struct struct_fmt struct_name ## _fmt = { \
        .s_name = #struct_name, \
        .num_members = VARCOUNT(__VA_ARGS__), \
        .struct_size = sizeof(struct_name), \
        .packed_size = (MAP(SIZE_OF_TYPE_ADD_PAIR,__VA_ARGS__) 0), \
        .offsets = (size_t[]){ MAP_LIST_UD(GET_OFFSET, struct_name, __VA_ARGS__)}, \
        .sizes = (size_t []){MAP_LIST(GET_SIZE_OF_PAIR, __VA_ARGS__)}, \
    };

size_t serialize(struct struct_fmt *fmt, void *structure, unsigned char *buffer);

size_t deserialize(struct struct_fmt *fmt, unsigned char *buffer, void *structure);

#endif
