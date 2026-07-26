#include "core/ecs.h"
#include "core/log.h"

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// TODO: IMPORTANT: error handling in case realloc doesn't work use tmp ptrs before moving pointer

typedef struct {
    signature_t  signature;
    uint64_t     count;          /* live rows                              */
    uint64_t*    entity_ids;     /* entity_ids[row] = entity               */
    void**       columns;        /* columns[comp_id-1] = packed col or NULL */
} archetype_t;

typedef struct {
    uint64_t  arch_idx;
    uint64_t  row;
    uint8_t   alive;
} entity_record_t;

static archetype_t*    archetypes       = NULL;
static uint64_t        arch_count       = 0;

static entity_record_t* entity_index     = NULL;

static size_t*         comp_sizes       = NULL;

static uint64_t ent_num  = 0;
static uint64_t comp_num = 0;

// Signature

static inline uint64_t sig_size() {
    return comp_num / CHAR_BIT + 1;
}

void add_sig(signature_t s1, const signature_t s2) {
    uint64_t n = sig_size();
    while (n--) s1[n] |= s2[n];
}

static void remove_sig(signature_t s1, const signature_t s2) {
    uint64_t n = sig_size();
    while (n--) s1[n] &= ~s2[n];
}

static int contains_sig(const signature_t s1, const signature_t s2) {
    uint64_t n = sig_size();
    while (n--) {
        if ((s1[n] & s2[n]) != s2[n]) return 0;
    }
    return 1;
}

static int excludes_sig(const signature_t s1, const signature_t s2) {
    uint64_t n = sig_size();
    while (n--) {
        if (s1[n] & s2[n]) return 0;
    }
    return 1;
}

static int sig_equal(const signature_t s1, const signature_t s2) {
    return memcmp(s1, s2, sig_size()) == 0;
}

static signature_t sig_alloc() {
    return calloc(sig_size(), sizeof(uint8_t));
}

static signature_t sig_clone(const signature_t src) {
    signature_t s = sig_alloc();
    memcpy(s, src, sig_size());
    return s;
}

signature_t id_to_sig(uint64_t id) {
    signature_t sig = sig_alloc();
    uint64_t n = sig_size();
    while (n--) {
        long curr = (long)id - (long)(n * CHAR_BIT);
        if (curr > 0) { sig[n] |= (uint8_t)(1 << (curr - 1)); break; }
    }
    return sig;
}

signature_t create_sig(uint32_t count, ...) {
    signature_t sig = sig_alloc();
    va_list args;
    va_start(args, count);
    for (uint32_t i = 0; i < count; i++) {
        uint64_t id = va_arg(args, uint64_t);
        if (id == 0) continue;
        uint64_t n = sig_size();
        while (n--) {
            long curr = (long)id - (long)(n * CHAR_BIT);
            if (curr > 0) { sig[n] |= (uint8_t)(1 << (curr - 1)); break; }
        }
    }
    va_end(args);
    return sig;
}

// DONE
uint64_t get_or_create_arch(signature_t sig) {
    for (uint64_t i = 0; i < arch_count; i++) {
        if (sig_equal(archetypes[i].signature, sig)) {
            return i;
        }
    }

    arch_count++;

    archetypes = realloc(archetypes, arch_count * sizeof(archetype_t));

    archetypes[arch_count - 1].signature = sig_clone(sig);
    archetypes[arch_count - 1].count = 0;
    archetypes[arch_count - 1].entity_ids = NULL;
    archetypes[arch_count - 1].columns = calloc(comp_num, sizeof(void*));

    return arch_count - 1;
}

// DONE
void arch_remove_row(archetype_t* arch, uint64_t row) {
    arch->count--;

    if (row != arch->count) {
        // move last entity_id into row
        arch->entity_ids[row] = arch->entity_ids[arch->count];
        
        // for all components move last one into row
        for (int i = 0; i < comp_num; i++) {
            signature_t comp_sig = id_to_sig(i + 1);

            if (contains_sig(arch->signature, comp_sig)) {
                memcpy(arch->columns[i] + row * comp_sizes[i], arch->columns[i] + arch->count * comp_sizes[i], comp_sizes[i]);
            }
            free(comp_sig);
        }
    }
}

// DONE
void move_entity(entity_t ent, signature_t sig) {
    // get or create arch
    uint64_t new_arch_id = get_or_create_arch(sig);

    archetype_t* old_arch = NULL;
    if (entity_index[ent].arch_idx != UINT64_MAX) {
        old_arch = &archetypes[entity_index[ent].arch_idx];
    }
    archetype_t* new_arch = &archetypes[new_arch_id];

    // allocate space for entity
    new_arch->count++;

    new_arch->entity_ids = realloc(new_arch->entity_ids, new_arch->count * sizeof(entity_t));

    // insert ent id into arch
    new_arch->entity_ids[new_arch->count - 1] = ent;
    entity_index[ent].arch_idx = new_arch_id;
    uint64_t old_row = entity_index[ent].row;
    uint64_t new_row = new_arch->count - 1;
    entity_index[ent].row = new_row;

    // allocate space for components
    for (int i = 0; i < comp_num; i++) {
        signature_t comp_sig = id_to_sig(i + 1);
        if (contains_sig(sig, comp_sig) && comp_sizes[i] != 0) {
            new_arch->columns[i] = realloc(new_arch->columns[i], new_arch->count * comp_sizes[i]);

            if (new_arch->columns[i] == NULL) {
                ERROR("Column realloc failed.");
            }

            if (old_arch != NULL) {
                if (contains_sig(old_arch->signature, comp_sig)) {
                    // copy component memory
                    memcpy(new_arch->columns[i] + new_row * comp_sizes[i], old_arch->columns[i] + old_row * comp_sizes[i], comp_sizes[i]);
                }
            }
        }
        free(comp_sig);
    }

    if (old_arch != NULL) {
        arch_remove_row(old_arch, old_row);
    }
}

// DONE
void add_comp(entity_t ent, uint64_t comp_id, void* data) {
    if (ent >= ent_num || !entity_index[ent].alive) return;
    if (comp_id == 0 || comp_id > comp_num)         return;

    signature_t sig = id_to_sig(comp_id);
    if (entity_index[ent].arch_idx != UINT64_MAX) {
        signature_t old_sig = archetypes[entity_index[ent].arch_idx].signature;
        add_sig(sig, old_sig);

        signature_t comp_sig = id_to_sig(comp_id);
        if (contains_sig(old_sig, comp_sig)) {
            TRACE("Entity %ld already contains component %ld", ent, comp_id);
            free(comp_sig);
            free(sig);
            return;
        }
        free(comp_sig);
    }

    move_entity(ent, sig);

    archetype_t* arch = &archetypes[entity_index[ent].arch_idx];
    memcpy(arch->columns[comp_id - 1] + entity_index[ent].row * comp_sizes[comp_id - 1], data, comp_sizes[comp_id - 1]);
    
    TRACE("Added component %ld to entity %ld", comp_id, ent);

    free(sig);
}

// DONE
uint64_t register_new_comp(size_t size) {
    comp_num++;

    comp_sizes = realloc(comp_sizes, comp_num * sizeof(size_t));

    comp_sizes[comp_num - 1] = size;

    for (int i = 0; i < arch_count; i++) {
        if (comp_num % CHAR_BIT == 1) {
            archetypes[i].signature = realloc(archetypes[i].signature, sig_size() * sizeof(uint8_t));
            archetypes[i].signature[sig_size() - 1] = 0;
        }
        archetypes[i].columns = realloc(archetypes[i].columns, comp_num * sizeof(void*));
        archetypes[i].columns[comp_num - 1] = NULL;
    }

    return comp_num;
}

// DONE
entity_t create_entity() {
    entity_t id;
    id = ent_num++;

    entity_index = realloc(entity_index, ent_num * sizeof(entity_record_t));

    entity_record_t* rec = &entity_index[id];
    rec->arch_idx = UINT64_MAX;
    rec->row   = 0;
    rec->alive = 1;

    TRACE("Create entity %ld.", id);
    return id;
}

// DONE
void remove_ent(entity_t ent) {
    if (ent >= ent_num || !entity_index[ent].alive) return;

    entity_record_t* rec = &entity_index[ent];

    arch_remove_row(&archetypes[rec->arch_idx], rec->row);

    rec->arch_idx = UINT64_MAX;
    rec->alive = 0;
}

// DONE
component_t get_comp(entity_t ent, uint64_t comp_id) {
    if (ent >= ent_num || !entity_index[ent].alive) {
        return NULL;
    }
    if (comp_id == 0 || comp_id > comp_num) {
        return NULL;
    }

    entity_record_t* rec = &entity_index[ent];
    if (rec->arch_idx == UINT64_MAX) {
        return NULL;
    }

    void* col = archetypes[rec->arch_idx].columns[comp_id - 1];
    if (col == NULL) {
        return NULL;
    }

    //TRACE("Retreived component %ld from entity %ld.", comp_id, ent);
    return col + rec->row * comp_sizes[comp_id - 1];
}

void remove_comp(entity_t ent, uint64_t comp_id) {
    if (ent >= ent_num || !entity_index[ent].alive) return;
    if (comp_id == 0 || comp_id > comp_num)         return;

    entity_record_t* rec = &entity_index[ent];
    if (rec->arch_idx == UINT64_MAX)                           return;

    // TODO: check if entity contains component
    
    uint64_t old_arch_id = rec->arch_idx;

    signature_t new_sig = sig_clone(archetypes[old_arch_id].signature);
    signature_t comp_sig = id_to_sig(comp_id);
    remove_sig(new_sig, comp_sig);
    free(comp_sig);

    move_entity(ent, new_sig);
    free(new_sig);
}

// DONE
entity_t* filter_entities(signature_t filter) {
    uint64_t len = 0;

    for (uint64_t i = 0; i < arch_count; i++) {
        if (contains_sig(archetypes[i].signature, filter)) {
            len += archetypes[i].count;
        }
    }

    entity_t* list = malloc((len + 1) * sizeof(entity_t));

    uint64_t idx = 0;
    for (uint64_t i = 0; i < arch_count; i++) {
        if (contains_sig(archetypes[i].signature, filter)) {
            memcpy(list + idx, archetypes[i].entity_ids, archetypes[i].count * sizeof(entity_t));
            idx += archetypes[i].count;
        }
    }

    char buff[100] = "";
    uint64_t n = sig_size();
    while (n--) {
        if (n == sig_size() - 1) {
            sprintf(buff,"%s%8.8B", buff, filter[n]);
            continue;
        }
        sprintf(buff,"%s %8.8B", buff, filter[n]);
    }

    TRACE("Filtered %ld entities with signature %s.", len, buff);

    list[len] = ENTITY_INVALID;
    return list;
}

entity_t* filter_entities_excluding(signature_t include, signature_t exclude) {
    uint64_t len = 0;

    for (uint64_t i = 0; i < arch_count; i++) {
        if (contains_sig(archetypes[i].signature, include) && excludes_sig(archetypes[i].signature, exclude)) {
            len += archetypes[i].count;
        }
    }

    entity_t* list = malloc((len + 1) * sizeof(entity_t));

    uint64_t idx = 0;
    for (uint64_t i = 0; i < arch_count; i++) {
        if (contains_sig(archetypes[i].signature, include) && excludes_sig(archetypes[i].signature, exclude)) {
            memcpy(list + idx, archetypes[i].entity_ids, archetypes[i].count * sizeof(entity_t));
            idx += archetypes[i].count;
        }
    }

    char inc[100] = "";
    uint64_t n = sig_size();
    while (n--) {
        if (n == sig_size() - 1) {
            sprintf(inc,"%s%8.8B", inc, include[n]);
            continue;
        }
        sprintf(inc,"%s %8.8B", inc, include[n]);
    }

    char exc[100] = "";
    n = sig_size();
    while (n--) {
        if (n == sig_size() - 1) {
            sprintf(exc,"%s%8.8B", exc, exclude[n]);
            continue;
        }
        sprintf(exc,"%s %8.8B", exc, exclude[n]);
    }

    TRACE("Filtered %ld entities with signature %s and excluding signature %s.", len, inc, exc);

    list[len] = ENTITY_INVALID;
    return list;
}

void cleanup_ecs() {
    // TODO: Free all memory
}
