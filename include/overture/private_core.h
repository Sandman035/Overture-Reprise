#ifndef OVERTURE_PRIVATE_CORE_H
#define OVERTURE_PRIVATE_CORE_H

#include <stdint.h>
#include <stddef.h>

typedef enum log_level_t log_level_t;

void print_log(log_level_t level, const char* file, int line, const char* fmt, ...);

typedef void(*system_ptr_t)(void);

typedef enum schedule_t schedule_t;

void register_system(system_ptr_t system, schedule_t schedule);
void register_system_front(system_ptr_t system, schedule_t schedule);

typedef uint8_t* signature_t;
typedef struct entity_t entity_t;

uint64_t register_new_comp();
void add_comp_cpy(entity_t* ent, uint64_t comp_id, void* data, size_t size);
void add_comp_store(entity_t* ent, uint64_t comp_id, void* data, size_t size);
signature_t id_to_sig(uint64_t id);
signature_t create_sig(uint32_t n, ...);
entity_t** filter_entities(signature_t filter);

#define X_ID(X) X ## _id
#define REGISTER_ID(X) \
    if (X ## _id == 0) { \
        X ## _id = register_new_comp(); \
        TRACE("Registered id %d for %s", X ## _id, #X); \
    }

#endif
