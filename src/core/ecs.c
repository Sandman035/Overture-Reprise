#include "core/ecs.h"
#include "core/log.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct entity_node_t {
    entity_t entity;
    struct entity_node_t* next;
} entity_node_t;

static entity_node_t* entity_head = NULL;
static entity_node_t* entity_tail = NULL;

static uint64_t ent_num = 0;
static uint64_t comp_num = 0;

// TODO: IMPORTANT: error handling in case realloc doesn't work use tmp ptrs before moving pointer

void add_sig(signature_t s1, const signature_t s2) {
    uint64_t n = comp_num / CHAR_BIT + 1;
    while (n--) {
        s1[n] |= s2[n];
    }
}

static void remove_sig(signature_t s1, const signature_t s2) {
    uint64_t n = comp_num / CHAR_BIT + 1;
    while (n--) {
        s1[n] &= ~s2[n];
    }
}

static int contains_sig(const signature_t s1, const signature_t s2) {
    uint64_t n = comp_num / CHAR_BIT + 1;

    int result = 1;
    while (n--) {
        int curr_result = 0;
        curr_result |= (s1[n] & s2[n]) == s2[n];
        result = result && curr_result;
    }
    return result;
}

signature_t id_to_sig(uint64_t id) {
    signature_t signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));
    uint64_t n = comp_num / CHAR_BIT + 1;
    while (n--) {
        long curr_num = id - (n * CHAR_BIT);
        if (curr_num > 0) {
            signature[n] |= (1 << (curr_num - 1));
            break;
        }
    }
    return signature;
}

signature_t create_sig(uint32_t n, ...) {
    signature_t signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));

    va_list args;
    va_start(args, n);

    for (uint32_t i = 0; i < n; ++i) {
        uint64_t n_char = comp_num / CHAR_BIT + 1;
        uint64_t current_id = va_arg(args, uint64_t);
        if (current_id != 0) {
            while (n_char--) {
                long curr_num = current_id - (n_char * CHAR_BIT);
                if (curr_num > 0) {
                    signature[n_char] |= (1 << (curr_num - 1));
                    break;
                }
            }
        }
    }

    va_end(args);

    return signature;
}

// DONE
uint64_t register_new_comp() {
    comp_num++;

    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        temp->entity.components = realloc(temp->entity.components, comp_num * sizeof(component_t));

        // maybe we should only realloc when signature overflows
        temp->entity.signature = realloc(temp->entity.signature, (comp_num / CHAR_BIT + 1) * sizeof(unsigned char));
        temp = temp->next;
    }

    //TRACE("Registered component %ld.", comp_num);

    return comp_num;
}

// passed a stack pointer
void add_comp_cpy(entity_t* ent, uint64_t comp_id, void *data, size_t size) {
    if (ent == NULL) {
        WARN("Entity does not exist, failed to add component %ld.", comp_id);
        return;
    }

    void* data_ptr = malloc(size);
    memcpy(data_ptr, data, size);
    ent->components[comp_id - 1] = data_ptr;

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = ent->signature;

    add_sig(ent_sig, comp_sig);

    free(comp_sig);

    TRACE("Added component %ld to entity %ld.", comp_id, ent->id);
}

// passed a heap pointer
void add_comp_store(entity_t* ent, uint64_t comp_id, void *data, size_t size) {
    if (ent == NULL) {
        WARN("Entity does not exist, failed to add component %ld.", comp_id);
        return;
    }

    ent->components[comp_id - 1] = data;

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = ent->signature;

    add_sig(ent_sig, comp_sig);

    free(comp_sig);

    TRACE("Added component %ld to entity %ld.", comp_id, ent->id);
}

// DONE
component_t get_comp(entity_t* ent, uint64_t comp_id) {
    if (ent == NULL) {
        WARN("Entity does not exist.");
        return NULL;
    }

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = ent->signature;

    int result = contains_sig(ent_sig, comp_sig);

    free(comp_sig);

    if (result) {
        TRACE("Retrieved component %ld from entity %ld.", comp_id, ent->id);
        return ent->components[comp_id - 1];
    }
    WARN("Entity %ld does not contain component %ld.", ent->id, comp_id);
    return NULL;
}

// DONE
void remove_comp(entity_t* ent, uint64_t comp_id) {
    if (ent == NULL) {
        WARN("Entity does not exist, cannot remove component %ld.", comp_id);
        return;
    }

    free(ent->components[comp_id - 1]);
    ent->components[comp_id - 1] = NULL;

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = ent->signature;

    remove_sig(ent_sig, comp_sig);

    free(comp_sig);

    TRACE("Removed component %ld from entity %ld", comp_id, ent->id);
}

// DONE? maybe entity needs to be malloc idk
entity_t* create_entity() {
    entity_node_t* node = malloc(sizeof(entity_node_t));
    node->entity.id = ent_num;
    node->entity.components = calloc(comp_num, sizeof(component_t));
    node->entity.signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));
    node->next = NULL;

    ent_num++;

    TRACE("Created entity %ld.", ent_num - 1);

    if (entity_head == NULL) {
        entity_head = node;
        entity_tail = node;
        return &node->entity;
    }

    entity_tail->next = node;
    entity_tail = node;

    return &node->entity;
}

// DONE
entity_t* get_ent(uint64_t id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->entity.id == id) {
            TRACE("Retrieved entity %ld.", id);
            return &temp->entity;
        }
        temp = temp->next;
    }

    ERROR("Entity %ld does not exist.", id);
    return NULL;
}

// DONE unless entity_t is malloc
void remove_ent(uint64_t id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->next->entity.id == id) {
            entity_node_t* ent = temp->next;

            uint64_t n = comp_num;
            while (n--) {
                free(ent->entity.components[n]);
                ent->entity.components[n] = NULL;
            }
            free(ent->entity.signature);
            ent->entity.signature = NULL;

            temp->next = ent->next;

            free(ent);
            ent = NULL;

            TRACE("Removed entity %ld.", id);
            return;
        }
        temp = temp->next;
    }

    WARN("Entity %ld does not exist, it cannot be removed.\n", id);
    return;
}

entity_t** filter_entities(signature_t filter) {
    uint64_t len = 0;
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (contains_sig(temp->entity.signature, filter)) {
            len += 1;
        }
        temp = temp->next;
    }

    entity_t** list = malloc((len + 1) * sizeof(entity_t));
    
    uint64_t idx = 0;
    temp = entity_head;
    while (temp != NULL) {
        if (contains_sig(temp->entity.signature, filter)) {
            list[idx++] = &temp->entity;
        }
        temp = temp->next;
    }

    list[idx++] = NULL;

    char buff[100] = "";
    uint64_t n = comp_num / CHAR_BIT + 1;
    while (n--) {
        if (n == comp_num / CHAR_BIT) {
            sprintf(buff,"%s%8.8B", buff, filter[n]);
            continue;
        }
        sprintf(buff,"%s %8.8B", buff, filter[n]);
    }

    TRACE("Filtered %ld entities with signature %s.", len, buff);

    return list;
}
