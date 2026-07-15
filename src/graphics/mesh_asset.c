#include "graphics/mesh_asset.h"
#include "assets/asset_manager.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/opengl.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define TOBJ_ENABLE_FILE_IO
#include <tiny_obj_c.h>

uint32_t load_tinyobj(const char* path, void** data_out, size_t* size, void* args) {
    tobj_scene_f scene;
    tobj_load_config cfg = tobj_default_config();
    tobj_diag diag = {0};

    if (tobj_load_obj_from_file_f(&scene, path, &cfg, &diag) != TOBJ_OK) {
        if (diag.err) {
            WARN("Failed to load model: %s: %s.", path, diag.err);
        }
        return 0;
    }

    // only get the first mesh for now
    const tobj_mesh_f *mesh = &scene.shapes[0].mesh;

    size_t vertex_count = mesh->num_indices;

    vertex_t* vertices = malloc(vertex_count * sizeof(vertex_t));
    uint32_t* indices = malloc(vertex_count * sizeof(uint32_t));

    for (size_t i = 0; i < vertex_count; i++) {
        indices[i] = i; // for now

        tobj_index idx = mesh->indices[i];

        vertices[i].pos.x = scene.attrib.vertices.ptr[3 * idx.vertex_index + 0];
        vertices[i].pos.y = scene.attrib.vertices.ptr[3 * idx.vertex_index + 1];
        vertices[i].pos.z = scene.attrib.vertices.ptr[3 * idx.vertex_index + 2];

        vertices[i].norm.x = scene.attrib.normals.ptr[3 * idx.normal_index + 0];
        vertices[i].norm.y = scene.attrib.normals.ptr[3 * idx.normal_index + 1];
        vertices[i].norm.z = scene.attrib.normals.ptr[3 * idx.normal_index + 2];

        vertices[i].tex.u = scene.attrib.texcoords.ptr[3 * idx.texcoord_index + 0];
        vertices[i].tex.v = scene.attrib.texcoords.ptr[3 * idx.texcoord_index + 1];
    }

    tobj_scene_free_f(&scene);
    tobj_diag_free(&diag, NULL);

    *size = sizeof(mesh_asset_t);

    *data_out = malloc(sizeof(mesh_asset_t));
    mesh_asset_t* mesh_asset = *data_out;

    mesh_asset->vertex_buffer = create_vertex_buffer(vertex_count * sizeof(vertex_t), (void*)vertices);
    instert_attrib(&mesh_asset->vertex_buffer, 0, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, pos));
    instert_attrib(&mesh_asset->vertex_buffer, 1, 3, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, norm));
    instert_attrib(&mesh_asset->vertex_buffer, 2, 2, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, tex));
    add_index_buffer(&mesh_asset->vertex_buffer, vertex_count * sizeof(uint32_t), (void*)indices);

    free(vertices);
    free(indices);

    return 1;
}

void unload_tinyobj() {
}

void setup_mesh_asset_loaders() {
    asset_loader_t tinyobj_loader;
    tinyobj_loader.load = load_tinyobj;
    tinyobj_loader.unload = unload_tinyobj;

    register_asset_loader("temp:tinyobj", tinyobj_loader);
}

REGISTER_SYSTEM(setup_mesh_asset_loaders, STARTUP);
