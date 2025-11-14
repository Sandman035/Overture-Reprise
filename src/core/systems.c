#include "core/systems.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct system_node_t {
    system_ptr_t system;
    struct system_node_t* next;
} system_node_t;

system_node_t* schedule_heads[NUM_OF_SCHEDULES];

// TODO: error handling for malloc
void register_system(system_ptr_t system, schedule_t schedule) {
    system_node_t* node = malloc(sizeof(system_node_t));
    node->system = system;
    node->next = NULL;

    if (schedule_heads[schedule] == NULL) {
        schedule_heads[schedule] = node;
        return;
    }

    system_node_t* temp = schedule_heads[schedule];
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = node;
}

void run_systems(schedule_t schedule) {
    system_node_t* temp = schedule_heads[schedule];
    while (temp != NULL) {
        temp->system();
        temp = temp->next;
    }
}
