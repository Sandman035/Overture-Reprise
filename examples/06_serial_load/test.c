#include <overture/overture.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/core/serialize_deserialize.h"

typedef struct test_t {
    uint32_t x;
    uint32_t y;
} test_t;

REGISTER_COMPONENT(test_t);

void* deserialize_test_t(serial_component_t comp) {
    test_t* test = malloc(sizeof(test_t));

    for (size_t i = 0; i < comp.data_count; i++) {
        if (strcmp(comp.data[i].name, "testx")) {
            test->x = *(uint32_t*)comp.data[i].data + 5;
        }
        if (strcmp(comp.data[i].name, "testy")) {
            test->y = *(uint32_t*)comp.data[i].data + 20;
        }
    }

    return test;
}

extern int should_exit;

void test() {
    uint32_t x = 5;
    uint32_t y = 10;

    serial_data_t data[2];

    data[0].data = &x;
    strcpy(data[0].name, "testx");
    data[1].data = &y;
    strcpy(data[1].name, "testy");

    serial_component_t comp;
    strcpy(comp.name, "test_t");
    comp.data_count = 2;
    comp.data = data;

    serial_entity_t ent;
    ent.comp_count = 1;
    ent.comps = &comp;

    serial_scene_tree_t tree;
    tree.ent_count = 1;
    tree.ents = &ent;

    actualize_scene_tree(tree);

    entity_t* list = FILTER_ENTITIES(test_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        test_t* test = get_comp(list[i], test_t_id);
        printf("x: %d, y: %d\n", test->x, test->y);
    }

    free(list);

    should_exit = 1;
}

REGISTER_SYSTEM(test, SETUP);
