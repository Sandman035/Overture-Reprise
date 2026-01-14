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
    "SETUP",
    "PRE_UPDATE",
    "UPDATE",
    "POST_UPDATE",
    "PRE_RENDER",
    "RENDER",
    "POST_RENDER",
    "CLEANUP",
    "POST_CLEANUP"
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

typedef struct thread_node_t {
    pthread_t thread;
    struct thread_node_t* next;
} thread_node_t;

void* run_system(void* arg) {
    system_ptr_t system = arg;
    system();
    return NULL;
}

void run_systems_parrallel(schedule_t schedule) {
    system_node_t* temp = schedule_heads[schedule];

    TRACE("Executing systems in schedule: %s.", schedules[schedule]);

    uint32_t count = 0;

    thread_node_t* thread_head = NULL;
    thread_node_t* curr_thread_node = thread_head;

    while (temp != NULL) {
        thread_node_t* new = malloc(sizeof(thread_node_t));
        pthread_create(&new->thread, NULL, run_system, temp->system);
        new->next = NULL;

        if (curr_thread_node == NULL) {
            curr_thread_node = new;
        } else {
            curr_thread_node->next = new;
            curr_thread_node = curr_thread_node->next;
        }
        temp = temp->next;
        count++;
    }

    TRACE("Waiting for system threads to finish...");

    while (curr_thread_node != NULL) {
        pthread_join(curr_thread_node->thread, NULL);

        thread_node_t* next = curr_thread_node->next;

        free(curr_thread_node);

        curr_thread_node = next;
    }

    TRACE("Finished execution of %d systems in schedule: %s.", count, schedules[schedule]);
}
