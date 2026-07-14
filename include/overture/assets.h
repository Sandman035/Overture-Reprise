#ifndef OVERTURE_ASSETS_H
#define OVERTURE_ASSETS_H

#include <stddef.h>
#include <stdint.h>

typedef struct asset_loader_t {
    uint32_t (*load)(const char* path, void** data_out, size_t* size, void* args);
    void (*unload)(/* asset data pointer */);
} asset_loader_t;

void register_asset_loader(const char* asset_type, asset_loader_t loader);

typedef uint64_t asset_handle_t;

asset_handle_t load_asset(const char* path, const char* asset_type, void* args);

asset_handle_t add_asset(void* data, size_t size, const char* asset_type);

void* get_asset(asset_handle_t handle);

#endif
