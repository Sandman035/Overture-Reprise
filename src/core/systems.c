#include "core/systems.h"
#include "core/log.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct system_node_t {
    system_ptr_t system;
    struct system_node_t* next;
} system_node_t;

system_node_t* schedule_heads[NUM_OF_SCHEDULES];

// maybe figure out a way to automatically update this with macros or smt
const char* schedules[] = {
    "SETUP",
    "PRE_UPDATE",
    "UPDATE",
    "POST_UPDATE",
    "RENDER",
    "CLEANUP"
};

// TODO: error handling for malloc
void register_system(system_ptr_t system, schedule_t schedule) {
    system_node_t* node = malloc(sizeof(system_node_t));
    node->system = system;
    node->next = NULL;

    if (schedule_heads[schedule] == NULL) {
        schedule_heads[schedule] = node;
        TRACE("Registered first system in schedule: %s.", schedules[schedule]);
        return;
    }

    system_node_t* temp = schedule_heads[schedule];
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = node;

    TRACE("Registered new system in schedule: %s.", schedules[schedule]);
}

void run_systems(schedule_t schedule) {
    system_node_t* temp = schedule_heads[schedule];
    TRACE("Executing systems in schedule: %s.", schedules[schedule]);
    while (temp != NULL) {
        temp->system();
        temp = temp->next;
    }
    TRACE("Finished execution of schedule: %s.", schedules[schedule]);
}
