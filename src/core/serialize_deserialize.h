#ifndef SERDE_OVERTURE
#define SERDE_OVERTURE

#include <stddef.h>
#include <stdint.h>

typedef struct serial_data_t {
    char name[30];
    size_t data_size;
    void* data;
} serial_data_t;

typedef struct serial_component_t {
    char name[30];
    size_t data_count;
    serial_data_t* data;
} serial_component_t;

typedef struct serial_entity_t {
    size_t comp_count;
    serial_component_t* comps;
} serial_entity_t;

typedef struct serial_scene_tree_t {
    char name[30];
    size_t ent_count;
    serial_entity_t* ents;
} serial_scene_tree_t;

void actualize_scene_tree(serial_scene_tree_t tree);

#endif
