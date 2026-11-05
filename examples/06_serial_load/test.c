#include <overture/overture.h>
#include <stdio.h>
#include <string.h>

#include "../src/core/serialize_deserialize.h"

typedef struct test_t {
    uint32_t x;
    float y;
} test_t;

REGISTER_COMPONENT(test_t);

void* deserialize_test_t(serial_component_t comp) {
    test_t* test = malloc(sizeof(test_t));

    for (size_t i = 0; i < comp.data_count; i++) {
        if (strcmp(comp.data[i].name, "testx") == 0) {
            test->x = *(uint32_t*)comp.data[i].data + 5;
        }
        if (strcmp(comp.data[i].name, "testy") == 0) {
            test->y = *(float*)comp.data[i].data;
        }
        if (strcmp(comp.data[i].name, "stringy") == 0) {
            DEBUG("%s", comp.data[i].data);
        }
        if (strcmp(comp.data[i].name, "arr") == 0) {
            DEBUG("hello");
            size_t n = comp.data[i].data_size / (sizeof(char)*100);
            DEBUG("%d", n);
            char (*strs)[100] = comp.data[i].data;
            for (size_t j = 0; j < n; j++) {
                DEBUG("%s", strs[j]);
            }
        }
    }

    return test;
}

extern int should_exit;

void test() {
    load_asset("assets/maps/test1.otm", "overture:text_map", NULL);

    entity_t* list = FILTER_ENTITIES(test_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        test_t* test = get_comp(list[i], test_t_id);
        printf("x: %d, y: %f\n", test->x, test->y);
    }

    free(list);

    //should_exit = 1;
}

REGISTER_SYSTEM(test, SETUP);
