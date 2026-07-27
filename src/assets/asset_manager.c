#include "assets/asset_manager.h"
#include "core/log.h"
#include <stdlib.h>
#include <string.h>

#define ASSET_LOADER_TABLE_SIZE 64
#define PATH_HASH_TABLE_SIZE 256

static uint64_t hash_asset_type(const char* asset_type) {
    // FNV-1a -- seems to be correct from minimal testing
    uint64_t hash = 0xcbf29ce484222325;

    for (const char* byte = asset_type; *byte != '\0'; ++byte) {
        hash = hash ^ *byte;
        hash = hash * 0x00000100000001b3;
    }

    return hash;
}

typedef struct path_asset_hash {
    uint64_t path_key;
    asset_handle_t handle;
    struct path_asset_hash* next;
} path_asset_hash;


typedef struct loader_node_t {
    uint64_t key;
    asset_loader_t loader;
    path_asset_hash* path_hash_table[PATH_HASH_TABLE_SIZE];
    struct loader_node_t* next;
} loader_node_t;

loader_node_t* asset_loader_table[ASSET_LOADER_TABLE_SIZE];

void register_asset_loader(const char* asset_type, asset_loader_t loader) {
    uint64_t hash = hash_asset_type(asset_type);
    size_t idx = hash % ASSET_LOADER_TABLE_SIZE;

    loader_node_t* node = malloc(sizeof(loader_node_t));

    node->key = hash;
    node->loader = loader;
    node->next = NULL;

    for (size_t i = 0; i < PATH_HASH_TABLE_SIZE; i++) {
        node->path_hash_table[i] = NULL;
    }

    if (asset_loader_table[idx] == NULL) {
        asset_loader_table[idx] = node;
        TRACE("Registered asset loader: \"%s\" with key: 0x%lx.", asset_type, hash);
        return;
    }

    loader_node_t* temp = asset_loader_table[idx];
    while (1) {
        if (temp->key == hash) {
            WARN("Asset loader for type: \"%s\" with key: 0x%lx already exists.", asset_type, hash);
            free(node);
            node = NULL;
            return;
        }

        if (temp->next == NULL) {
            break;
        }

        temp = temp->next;
    }


    temp->next = node;
    TRACE("Registered asset loader: \"%s\" with key: 0x%lx.", asset_type, hash);
}

// TODO: parallelize once i finally have a good parallelization system
// NOTE: what if someone wants to load the same asset twice but different args????
asset_handle_t load_asset(const char* path, const char* asset_type, void* args) {
    uint64_t key = hash_asset_type(asset_type);
    size_t idx = key % ASSET_LOADER_TABLE_SIZE;

    loader_node_t* loader = asset_loader_table[idx];
    while (loader != NULL) {
        if (loader->key == key) {
            break;
        }
        loader = loader->next;
    }

    if (loader->key != key) {
        WARN("No asset loader for type: %s with key: 0x%lx. Cannot load asset: %s.", asset_type, key, path);
        return 0;
    }

    void* data;
    size_t size;
    if (!loader->loader.load(path, &data, &size, args)) {
        WARN("Failed to load asset: %s.", path);
        free(data);
        return 0;
    }

    uint64_t hash = hash_asset_type(path);
    size_t path_idx = hash % PATH_HASH_TABLE_SIZE;

    path_asset_hash* item = malloc(sizeof(path_asset_hash));
    item->next = NULL;
    item->path_key = hash;

    if (loader->path_hash_table[path_idx] == NULL) {
        loader->path_hash_table[path_idx] = item;
    } else {
        path_asset_hash* temp = loader->path_hash_table[path_idx];
        while (temp != NULL) {
            if (temp->path_key == hash) {
                free(item);
                free(data);
                return temp->handle;
            }

            if (temp->next == NULL) {
                break;
            }

            temp = temp->next;
        }

        temp->next = item;
    }

    asset_handle_t handle = add_asset(data, size, asset_type);

    item->handle = handle;

    free(data); // NOTE: maybe just store the pointer in add_asset, would require two versions of the function tho
    data = NULL;

    return handle;
}

// TODO: maybe rework for better/optimized memory management, to be fair this is not a performance critical thing

typedef struct {
    void* data;
    size_t size;
} asset_slot_t;

asset_slot_t* asset_slots = NULL;
uint64_t asset_count = 0;

asset_handle_t add_asset(void *data, size_t size, const char *asset_type) {
    asset_count++;

    asset_slots = realloc(asset_slots, asset_count * sizeof(asset_slot_t));

    asset_slots[asset_count - 1].data = malloc(size);
    memcpy(asset_slots[asset_count - 1].data, data, size);

    asset_slots[asset_count - 1].size = size;

    // TODO: store asset type and once parallelization then a ready "bool"

    return asset_count;
}

void* get_asset(asset_handle_t handle) {
    if (handle == 0) {
        WARN("Asset undefined.");
        return NULL;
    }

    if (handle > asset_count) {
        WARN("Asset %ld does not exist.");
        return NULL;
    }
    return asset_slots[handle - 1].data;
}
