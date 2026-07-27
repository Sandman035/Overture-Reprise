#include "graphics/texture_asset.h"

#include "assets/asset_manager.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/opengl.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

uint32_t load_stb_image(const char* path, void** data_out, size_t* size, void* args) {
    int32_t width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data == NULL) {
        WARN("Could not load texture: %s", path);
        return 0;
    }

    *data_out = malloc(sizeof(texture_asset_t));
    texture_asset_t* texture = *data_out;

    GLenum format;
    GLenum internal_format;

    switch (nrChannels) {
        case 1:
            format = GL_RED;
            internal_format = GL_R8;
            break;

        case 2:
            format = GL_RG;
            internal_format = GL_RG8;
            break;

        case 3:
            format = GL_RGB;
            internal_format = GL_RGB8;
            break;

        case 4:
            format = GL_RGBA;
            internal_format = GL_RGBA8;
            break;

        default:
            WARN("Texture format not supported.");
            return 0;
    }

    texture->texture = create_texture(width, height, data, internal_format, format);

    stbi_image_free(data);

    TRACE("Created Texture.");

    return 1;
}

void unload_stb_image() {
    // TODO: clear texture mem
}

void setup_texture_asset_loaders() {
    asset_loader_t stb_image_loader;
    stb_image_loader.load = load_stb_image;
    stb_image_loader.unload = unload_stb_image;

    register_asset_loader("overture:stb_image", stb_image_loader);
}

REGISTER_SYSTEM(setup_texture_asset_loaders, STARTUP);
