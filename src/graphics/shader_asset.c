#include "graphics/shader_asset.h"
#include "assets/asset_manager.h"
#include "core/systems.h"
#include "core/log.h"
#include "platform/fs.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t load_glsl(const char* path, void** data_out, size_t* size, void* args) {
    glsl_shader_loader_args_t* loader_args = args;

    if (args == NULL) {
        WARN("No arguments provided, unaible to load shader.");
        return 0;
    }

    size_t file_size;
    const char* shader_source = read_file(path, &file_size);

    if (shader_source == NULL) {
        WARN("Could not load shader source.");
        return 0;
    }

    uint32_t shader = glCreateShader(loader_args->type);
    const int32_t glint_size = file_size;
    glShaderSource(shader, 1, &shader_source, &glint_size);
    glCompileShader(shader);

    int32_t success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        WARN("Shader compilation failed: %s.", info_log);
        return 0;
    }

    *size = sizeof(shader_asset_t);

    *data_out = malloc(sizeof(shader_asset_t));
    shader_asset_t* shader_asset = *data_out;
    shader_asset->type = loader_args->type;
    shader_asset->shader_id = shader;

    TRACE("Created shader.");

    free((void*)shader_source);
    shader_source = NULL;

    return 1;
}

void unload_glsl() {
    // TODO: clear shader mem
}

void setup_shader_asset_loaders() {
    asset_loader_t glsl_fragment_loader;
    glsl_fragment_loader.load = load_glsl;
    glsl_fragment_loader.unload = unload_glsl;

    register_asset_loader("overture:glsl_shader", glsl_fragment_loader);
}

REGISTER_SYSTEM(setup_shader_asset_loaders, STARTUP);
