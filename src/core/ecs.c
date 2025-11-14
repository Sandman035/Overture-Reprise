#include "core/ecs.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct entity_node_t {
    unsigned long id;
    entity_t entity;
    signature_t signature;
    struct entity_node_t* next;
} entity_node_t;

entity_node_t* entity_head = NULL;

static unsigned long ent_num = 0;
static unsigned long comp_num = 0;

// TODO: IMPORTANT: error handling in case realloc doesn't work use tmp ptrs before moving pointer

void add_sig(signature_t s1, const signature_t s2) {
    int n = comp_num / CHAR_BIT + 1;
    while (n--) {
        s1[n] |= s2[n];
    }
}

static void remove_sig(signature_t s1, const signature_t s2) {
    int n = comp_num / CHAR_BIT + 1;
    while (n--) {
        s1[n] &= ~s2[n];
    }
}

static int contains_sig(const signature_t s1, const signature_t s2) {
    int n = comp_num / CHAR_BIT + 1;
    int result = 0;
    while (n--) {
        result |= s1[n] & s2[n];
    }
    return result;
}

// TEST: larger than 8 values for id, need to have bigger signature as well
signature_t id_to_sig(unsigned long id) {
    signature_t signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));
    int n = comp_num / CHAR_BIT + 1;
    while (n--) {
        unsigned long curr_num = id - (n * CHAR_BIT);
        if (curr_num > 0) {
            signature[n] |= (1 << (curr_num - 1));
            break;
        }
    }
    return signature;
}

signature_t create_sig(int n, ...) {
    signature_t signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));

    va_list args;
    va_start(args, n);

    for (int i = 0; i < n; ++i) {
        int n = comp_num / CHAR_BIT + 1;
        while (n--) {
            unsigned long curr_num = va_arg(args, unsigned long) - (n * CHAR_BIT);
            if (curr_num > 0) {
                signature[n] |= (1 << (curr_num - 1));
                break;
            }
        }
    }

    va_end(args);

    return signature;
}

unsigned long register_new_comp() {
    comp_num++;

    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        temp->entity = realloc(temp->entity, comp_num * sizeof(component_t));

        // maybe we should only realloc when signature overflows
        temp->signature = realloc(temp->signature, (comp_num / CHAR_BIT + 1) * sizeof(unsigned char));
        temp = temp->next;
    }

    return comp_num;
}

void add_comp(unsigned long ent_id, unsigned long comp_id, void *data, size_t size) {
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

            return;
        }
        temp = temp->next;
    }

    printf("Entity %ld does not exist.\n", ent_id);
    return;
}

component_t get_comp(unsigned long ent_id, unsigned long comp_id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == ent_id) {
            signature_t comp_sig = id_to_sig(comp_id);
            signature_t ent_sig = temp->signature;

            int result = contains_sig(ent_sig, comp_sig);

            free(comp_sig);

            if (result) {
                return temp->entity[comp_id - 1];
            }
        }
        temp = temp->next;
    }

    return NULL;
}

component_t get_comp_from_ent(entity_t ent, unsigned long comp_id) {
    return ent[comp_id - 1];
}

void remove_comp(unsigned long ent_id, unsigned long comp_id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == ent_id) {
            free(temp->entity[comp_id - 1]);
            temp->entity[comp_id - 1] = NULL;

            signature_t comp_sig = id_to_sig(comp_id);
            signature_t ent_sig = temp->signature;

            remove_sig(ent_sig, comp_sig);

            free(comp_sig);

            return;
        }
        temp = temp->next;
    }

    printf("Entity %ld does not exist.\n", ent_id);
    return;
}

int has_comp(unsigned long ent_id, unsigned long comp_id) {
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

    printf("Entity %ld does not exist.\n", ent_id);
    return 0;
}

/// returns new entity index
unsigned long add_ent() {
    entity_node_t* node = malloc(sizeof(entity_node_t));
    node->id = ent_num;
    node->entity = calloc(comp_num, sizeof(component_t));
    node->signature = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));
    node->next = NULL;

    ent_num++;

    if (entity_head == NULL) {
        entity_head = node;
        return ent_num - 1;
    }

    entity_node_t* temp = entity_head;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = node;

    return ent_num - 1;
}

entity_t get_ent(unsigned long id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->id == id) {
            return temp->entity;
        }
        temp = temp->next;
    }

    printf("Entity %ld does not exist.\n", id);
    return NULL;
}

void remove_ent(unsigned long id) {
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (temp->next->id == id) {
            entity_node_t* ent = temp->next;

            unsigned long n = comp_num;
            while (n--) {
                free(ent->entity[n]);
                ent->entity[n] = NULL;
            }
            free(ent->signature);
            ent->signature = NULL;

            temp->next = ent->next;

            free(ent);
            ent = NULL;
            return;
        }
        temp = temp->next;
    }

    printf("Entity %ld does not exist.\n", id);
    return;
}

entity_t* filter_entities(signature_t filter) {
    unsigned long len = 0;
    entity_node_t* temp = entity_head;
    while (temp != NULL) {
        if (contains_sig(temp->signature, filter)) {
            len += 1;
        }
        temp = temp->next;
    }

    entity_t* list = malloc((len + 1) * sizeof(entity_t));
    
    unsigned long idx = 0;
    temp = entity_head;
    while (temp != NULL) {
        if (contains_sig(temp->signature, filter)) {
            list[idx++] = temp->entity;
        }
        temp = temp->next;
    }

    list[idx++] = NULL;

    return list;
}
