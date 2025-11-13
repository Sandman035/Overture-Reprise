#include "core/systems.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct system_node_t {
    system_ptr_t system;
    struct system_node_t* next;
} system_node_t;

system_node_t** schedule_heads = NULL;

typedef struct system_queue_node_t {
    system_ptr_t system;
    schedule_t schedule;
    struct system_queue_node_t* next;
} system_queue_node_t;

system_queue_node_t* system_queue_head = NULL;

// TODO: error handling for malloc
void init_systems() {
    schedule_heads = calloc(NUM_OF_SCHEDULES, sizeof(system_node_t*));

    while (system_queue_head != NULL) {
        register_system(system_queue_head->system, system_queue_head->schedule);
        system_queue_node_t* next = system_queue_head->next;
        free(system_queue_head);
        system_queue_head = next;
    }
}

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

void add_system(system_ptr_t system, schedule_t schedule) {
    system_queue_node_t* node = malloc(sizeof(system_queue_node_t));
    node->system = system;
    node->schedule = schedule;
    node->next = NULL;

    if (system_queue_head == NULL) {
        system_queue_head = node;
        return;
    }

    system_queue_node_t* temp = system_queue_head;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = node;
}
