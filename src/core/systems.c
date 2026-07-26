#include "core/systems.h"
#include "core/log.h"

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct system_node_t {
    system_ptr_t system;
    struct system_node_t* next;
} system_node_t;

static system_node_t* schedule_heads[NUM_OF_SCHEDULES];

// maybe figure out a way to automatically update this with macros or smt
const char* schedules[] = {
    "STARTUP",
    "SETUP",
    "PRE_UPDATE",
    "UPDATE",
    "POST_UPDATE",
    "PRE_RENDER",
    "RENDER",
    "POST_RENDER",
    "CLEANUP",
};

// TODO: error handling for malloc
void register_system(system_ptr_t system, schedule_t schedule) {
    system_node_t* node = malloc(sizeof(system_node_t));
    node->system = system;
    node->next = NULL;

    TRACE("Registered system %p in schedule: %s.", system, schedules[schedule]);
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

void register_system_front(system_ptr_t system, schedule_t schedule) {
    system_node_t* node = malloc(sizeof(system_node_t));
    node->system = system;
    node->next = NULL;

    TRACE("Registered system %p in schedule: %s.", system, schedules[schedule]);
    if (schedule_heads[schedule] == NULL) {
        schedule_heads[schedule] = node;
        return;
    }

    system_node_t* temp = schedule_heads[schedule];

    node->next = temp;
    schedule_heads[schedule] = node;
}

void register_system_before(system_ptr_t system, system_ptr_t target, schedule_t schedule) {
    system_node_t* node = malloc(sizeof(system_node_t));
    node->system = system;
    node->next = NULL;

    TRACE("Registered system %p in schedule: %s.", system, schedules[schedule]);
    if (schedule_heads[schedule] == NULL) {
        schedule_heads[schedule] = node;
        return;
    }

    system_node_t* temp = schedule_heads[schedule];
    while (temp->next != NULL || temp->next->system != target) {
        temp = temp->next;
    }

    node->next = temp->next;
    temp->next = node;
}

void run_systems_sequential(schedule_t schedule) {
    system_node_t* temp = schedule_heads[schedule];

    TRACE("Executing systems in schedule: %s.", schedules[schedule]);

    uint32_t count = 0;
    while (temp != NULL) {
        temp->system();
        temp = temp->next;
        count++;
    }

    TRACE("Finished execution of %d systems in schedule: %s.", count, schedules[schedule]);
}

void run_systems_parrallel(schedule_t schedule) {
    ERROR("PARRALLEL SYSTEMS NOT IMPLEMENTED YET.");
}
