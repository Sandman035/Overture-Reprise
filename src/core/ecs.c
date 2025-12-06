#include "core/ecs.h"
#include "core/log.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct entity_node_t {
    uint64_t id;
    entity_t entity;
    signature_t signature;
    struct entity_node_t* next;
} entity_node_t;

static entity_node_t* entity_head = NULL;

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
    int result = 0;
    while (n--) {
        result |= s1[n] & s2[n];
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
        while (n_char--) {
            long curr_num = current_id - (n_char * CHAR_BIT);
            if (curr_num > 0) {
                signature[n_char] |= (1 << (curr_num - 1));
                break;
            }
        }
    }

    va_end(args);

    return signature;
}

uint64_t register_new_comp() {
    comp_num++;

    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        temp->entity = realloc(temp->entity, comp_num * sizeof(component_t));

        // maybe we should only realloc when signature overflows
        temp->signature = realloc(temp->signature, (comp_num / CHAR_BIT + 1) * sizeof(unsigned char));
        temp = temp->next;
    }

    TRACE("Registered component %ld.", comp_num);

    return comp_num;
}

void add_comp(uint64_t ent_id, uint64_t comp_id, void *data, size_t size) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == ent_id) {
            //malloc and memcpy might not be necessary although its a good safeguard maybe idk
            void* data_ptr = malloc(size);
            memcpy(data_ptr, data, size);
            temp->entity[comp_id - 1] = data_ptr;

            signature_t comp_sig = id_to_sig(comp_id);
            signature_t ent_sig = temp->signature;

            add_sig(ent_sig, comp_sig);

            free(comp_sig);

            TRACE("Added component %ld to entity %ld.", comp_id, ent_id);

            return;
        }
        temp = temp->next;
    }

    ERROR("Entity %ld does not exist, failed to add component %ld.", ent_id, comp_id);
    return;
}

component_t get_comp(uint64_t ent_id, uint64_t comp_id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == ent_id) {
            signature_t comp_sig = id_to_sig(comp_id);
            signature_t ent_sig = temp->signature;

            int result = contains_sig(ent_sig, comp_sig);

            free(comp_sig);

            if (result) {
                TRACE("Retrieved component %ld from entity %ld.", comp_id, ent_id);
                return temp->entity[comp_id - 1];
            }
            ERROR("Entity %ld does not contain component %ld.", ent_id, comp_id);
        }
        temp = temp->next;
    }

    ERROR("Entity %ld does not exist.", ent_id, comp_id);
    return NULL;
}

component_t get_comp_from_ent(entity_t ent, uint64_t comp_id) {
    return ent[comp_id - 1];
}

void remove_comp(uint64_t ent_id, uint64_t comp_id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == ent_id) {
            free(temp->entity[comp_id - 1]);
            temp->entity[comp_id - 1] = NULL;

            signature_t comp_sig = id_to_sig(comp_id);
            signature_t ent_sig = temp->signature;

            remove_sig(ent_sig, comp_sig);

            free(comp_sig);

            TRACE("Removed component %ld from entity %ld", comp_id, ent_id);

            return;
        }
        temp = temp->next;
    }

    WARN("Entity %ld does not exist, cannot remove component %ld.", ent_id, comp_id);
    return;
}

int has_comp(uint64_t ent_id, uint64_t comp_id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == ent_id) {
            signature_t comp_sig = id_to_sig(comp_id);
            signature_t ent_sig = temp->signature;

            int result = contains_sig(ent_sig, comp_sig);

            free(comp_sig);

            return result;
        }
        temp = temp->next;
    }

    ERROR("Entity %ld does not exist.", ent_id);
    return 0;
}

/// returns new entity index
uint64_t add_ent() {
    entity_node_t* node = malloc(sizeof(entity_node_t));
    node->id = ent_num;
    node->entity = calloc(comp_num, sizeof(component_t));
    node->signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));
    node->next = NULL;

    ent_num++;

    if (entity_head == NULL) {
        TRACE("Created first entity.");
        entity_head = node;
        return ent_num - 1;
    }

    entity_node_t* temp = entity_head;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = node;

    TRACE("Created entity %ld.", ent_num - 1);

    return ent_num - 1;
}

entity_t get_ent(uint64_t id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == id) {
            TRACE("Retrieved entity %ld.", id);
            return temp->entity;
        }
        temp = temp->next;
    }

    ERROR("Entity %ld does not exist.", id);
    return NULL;
}

void remove_ent(uint64_t id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->next->id == id) {
            entity_node_t* ent = temp->next;

            uint64_t n = comp_num;
            while (n--) {
                free(ent->entity[n]);
                ent->entity[n] = NULL;
            }
            free(ent->signature);
            ent->signature = NULL;

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

entity_t* filter_entities(signature_t filter) {
    uint64_t len = 0;
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (contains_sig(temp->signature, filter)) {
            len += 1;
        }
        temp = temp->next;
    }

    entity_t* list = malloc((len + 1) * sizeof(entity_t));
    
    uint64_t idx = 0;
    temp = entity_head;
    while (temp != NULL) {
        if (contains_sig(temp->signature, filter)) {
            list[idx++] = temp->entity;
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
