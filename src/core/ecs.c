#include "core/ecs.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static entity_t* entity_list = NULL;
static signature_t* signature_list = NULL;

static unsigned long ent_num = 0;
static unsigned long comp_num = 0;

// TODO: IMPORTANT: error handling in case realloc doesn't work use tmp ptrs before moving pointer

// idk if const does anything due to pointer nature of signature_t
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

unsigned long register_new_comp() {
    comp_num++;

    for (unsigned long i = 0; i < ent_num; i++) {
        if (entity_list[i] == NULL) {
            continue;
        }

        entity_list[i] = realloc(entity_list[i], comp_num * sizeof(component_t));

        // maybe we should only realloc when signature overflows
        signature_list[i] = realloc(signature_list[i], (comp_num / CHAR_BIT + 1) * sizeof(unsigned char));
    }

    return comp_num;
}

void add_comp(unsigned long ent_id, unsigned long comp_id, void *data, size_t size) {
    if (entity_list[ent_id] == NULL || ent_id > ent_num) {
        printf("Entity %ld does not exist.\n", ent_id);
        return;
    }
    //malloc and memcpy might not be necessary although its a good safeguard maybe idk
    void* data_ptr = malloc(size);
    memcpy(data_ptr, data, size);
    entity_list[ent_id][comp_id - 1] = data_ptr;

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = signature_list[ent_id];

    add_sig(ent_sig, comp_sig);

    free(comp_sig);
}

component_t get_comp(unsigned long ent_id, unsigned long comp_id) {
    if (has_comp(ent_id, comp_id)) {
        return entity_list[ent_id][comp_id - 1];
    }

    return NULL;
}

component_t get_comp_from_ent(entity_t ent, unsigned long comp_id) {
    return ent[comp_id - 1];
}

void remove_comp(unsigned long ent_id, unsigned long comp_id) {
    if (entity_list[ent_id] == NULL || ent_id > ent_num) {
        printf("Entity %ld does not exist.\n", ent_id);
        return;
    }

    free(entity_list[ent_id][comp_id - 1]);
    entity_list[ent_id][comp_id - 1] = NULL;

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = signature_list[ent_id];

    remove_sig(ent_sig, comp_sig);

    free(comp_sig);
}

int has_comp(unsigned long ent_id, unsigned long comp_id) {
    if (entity_list[ent_id] == NULL || ent_id > ent_num) {
        printf("Entity %ld does not exist.\n", ent_id);
        return 0;
    }

    signature_t comp_sig = id_to_sig(comp_id);
    signature_t ent_sig = signature_list[ent_id];

    int result = contains_sig(ent_sig, comp_sig);

    free(comp_sig);

    return result;
}

/// returns new entity index
unsigned long add_ent() {
    ent_num++;

    entity_list = realloc(entity_list, ent_num * sizeof(entity_t));
    entity_list[ent_num - 1] = calloc(comp_num, sizeof(component_t));

    signature_list = realloc(signature_list, ent_num * sizeof(entity_t));
    signature_list[ent_num - 1] = calloc(comp_num / CHAR_BIT + 1, sizeof(unsigned char));

    return ent_num - 1;
}

entity_t get_ent(unsigned long id) {
    if (entity_list[id] == NULL || id > ent_num) {
        printf("Entity %ld does not exist.\n", id);
        return NULL;
    }
    return entity_list[id];
}

void remove_ent(unsigned long id) {
    if (entity_list[id] == NULL || id > ent_num) {
        printf("Entity %ld does not exist.\n", id);
        return;
    }
    
    unsigned long n = comp_num;
    while (n--) {
        // TEST: remove entity partially filled with components
        // maybe previous calloc wont work we'll see
        free(entity_list[id][n]);
        entity_list[id][n] = NULL;
    }

    free(entity_list[id]);
    entity_list[id] = NULL;
}

entity_t* filter_entities(signature_t filter) {
    unsigned long len = 0;
    for (unsigned long i = 0; i < ent_num; i++) {
        if (contains_sig(signature_list[i], filter)) {
            len += 1;
        }
    }

    entity_t* list = malloc((len + 1) * sizeof(entity_t));
    
    unsigned long idx = 0;
    for (unsigned long i = 0; i < ent_num; i++) {
        if (contains_sig(signature_list[i], filter)) {
            list[idx++] = entity_list[i];
        }
    }

    list[idx++] = NULL;

    return list;
}
