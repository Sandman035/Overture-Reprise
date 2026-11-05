#include "assets/asset_manager.h"
#include "core/log.h"
#include "core/serialize_deserialize.h"
#include "core/systems.h"
#include "platform/fs.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t parse_array(const char* s, serial_scene_tree_t* tree) {
    uint8_t type = 0;
    const char* p = s;
    char name[30];
    int consumed = 0;
    if (sscanf(p, "str: %29s%n", name, &consumed) == 1) {
        type = 1;
    } else if (sscanf(p, "f32: %29s%n", name, &consumed) == 1) {
        type = 2;
    } else {
        return 0;
    }
    p += consumed;

    while (isspace((unsigned char)*p)) {
        p++;
    }

    if (*p != '[') {
        return 0;
    }

    p++;

    serial_entity_t* curr_ent = &tree->ents[tree->ent_count - 1];
    serial_component_t* curr_comp = &curr_ent->comps[curr_ent->comp_count - 1];
    curr_comp->data_count++;
    curr_comp->data = realloc(curr_comp->data, curr_comp->data_count * sizeof(serial_data_t));
    strcpy(curr_comp->data[curr_comp->data_count - 1].name, name);

    char (*strings)[100] = NULL;
    char str[100];
    float* floats = NULL;
    float f;

    size_t n = 0;
    while(1) {
        while (isspace((unsigned char)*p) || *p == ',') p++;
        if (*p == ']') break;

        if (sscanf(p, "\"%99[^\"]\"%n", str, &consumed) == 1 && type == 1) {
            n++;
            strings = realloc(strings, sizeof(char)*100*n);
            strcpy(strings[n - 1], str);

            p += consumed;
        }

        if (sscanf(p, "%f%n", &f, &consumed) == 1 && type == 2) {
            n++;
            floats = realloc(floats, sizeof(float)*n);
            floats[n-1] = f;

            p += consumed;
        }
    }

    if (type == 1) {
        curr_comp->data[curr_comp->data_count - 1].data = strings;
        curr_comp->data[curr_comp->data_count - 1].data_size = sizeof(char)*100*n;
    }

    if (type == 2) {
        curr_comp->data[curr_comp->data_count - 1].data = floats;
        curr_comp->data[curr_comp->data_count - 1].data_size = sizeof(float)*n;
    }

    return 1;
}

uint32_t load_text_map(const char* path, void** data_out, size_t* size, void* args) {
    serial_scene_tree_t* tree = malloc(sizeof(serial_scene_tree_t));
    *data_out = tree;
    *size = sizeof(serial_scene_tree_t);
    tree->ent_count = 0;
    tree->ents = NULL;

    size_t file_size;
    const char* file = read_file(path, &file_size);

    if (file == NULL) {
        WARN("Could not load map file.");
        return 0;
    }

    const char* p = file;
    char line[256];

    serial_entity_t* curr_ent = NULL;

    while(*p) {
        size_t len = strcspn(p, "\n");
        size_t n = len < sizeof line - 1 ? len : sizeof line - 1;
        memcpy(line, p, n);
        line[n] = '\0';
        p += len;
        if (*p == '\n') p++;

        char* s = line;
        if (*s == '\0') continue;
 
        char name[30];
 
        if (strcmp(s, "ent:") == 0) {
            serial_entity_t ent;
            ent.comp_count = 0;
            ent.comps = NULL;

            tree->ent_count++;
            tree->ents = realloc(tree->ents, tree->ent_count * sizeof(serial_entity_t));
            tree->ents[tree->ent_count - 1] = ent;
            curr_ent = &tree->ents[tree->ent_count - 1];
        } else if (curr_ent != NULL) {
            if (sscanf(s, "comp: %29s", name) == 1) {
                curr_ent->comp_count++;
                curr_ent->comps = realloc(curr_ent->comps, curr_ent->comp_count * sizeof(serial_component_t));

                strcpy(curr_ent->comps[curr_ent->comp_count - 1].name, name);
                curr_ent->comps[curr_ent->comp_count - 1].data = NULL;
                curr_ent->comps[curr_ent->comp_count - 1].data_count = 0;
            } else if (curr_ent->comps != NULL){
                uint32_t u;
                int32_t i;
                float f;
                char str[100];

                if (sscanf(s, "u32: %29s %u", name, &u) == 2) {
                    serial_component_t* curr_comp = &curr_ent->comps[curr_ent->comp_count - 1];
                    curr_comp->data_count++;
                    curr_comp->data = realloc(curr_comp->data, curr_comp->data_count * sizeof(serial_data_t));

                    strcpy(curr_comp->data[curr_comp->data_count - 1].name, name);
                    uint32_t* u32 = malloc(sizeof(uint32_t));
                    *u32 = u;
                    curr_comp->data[curr_comp->data_count - 1].data = u32;
                    curr_comp->data[curr_comp->data_count - 1].data_size = sizeof(uint32_t);
                } else if (sscanf(s, "i32: %29s %d", name, &i) == 2) {
                    serial_component_t* curr_comp = &curr_ent->comps[curr_ent->comp_count - 1];
                    curr_comp->data_count++;
                    curr_comp->data = realloc(curr_comp->data, curr_comp->data_count * sizeof(serial_data_t));

                    strcpy(curr_comp->data[curr_comp->data_count - 1].name, name);
                    int32_t* i32 = malloc(sizeof(int32_t));
                    *i32 = i;
                    curr_comp->data[curr_comp->data_count - 1].data = i32;
                    curr_comp->data[curr_comp->data_count - 1].data_size = sizeof(int32_t);
                } else if (sscanf(s, "f32: %29s %f", name, &f) == 2) {
                    serial_component_t* curr_comp = &curr_ent->comps[curr_ent->comp_count - 1];
                    curr_comp->data_count++;
                    curr_comp->data = realloc(curr_comp->data, curr_comp->data_count * sizeof(serial_data_t));

                    strcpy(curr_comp->data[curr_comp->data_count - 1].name, name);
                    float* f32 = malloc(sizeof(float));
                    *f32 = f;
                    curr_comp->data[curr_comp->data_count - 1].data = f32;
                    curr_comp->data[curr_comp->data_count - 1].data_size = sizeof(float);
                } else if (sscanf(s, "str: %29s \"%99[^\"]\"", name, str) == 2) {
                    serial_component_t* curr_comp = &curr_ent->comps[curr_ent->comp_count - 1];
                    curr_comp->data_count++;
                    curr_comp->data = realloc(curr_comp->data, curr_comp->data_count * sizeof(serial_data_t));

                    strcpy(curr_comp->data[curr_comp->data_count - 1].name, name);
                    char* string = malloc(sizeof(char) * 100);
                    strcpy(string, str);
                    curr_comp->data[curr_comp->data_count - 1].data = string;
                    curr_comp->data[curr_comp->data_count - 1].data_size = sizeof(char)*100;
                } else if (parse_array(s, tree) == 1) {
                    // nothing
                }
            }
        }
    }

    // this is temp
    actualize_scene_tree(*tree);

    return 1;
}

void unload_text_map() {
}

void setup_map_asset_loaders() {
    asset_loader_t text_map_loader;
    text_map_loader.load = load_text_map;
    text_map_loader.unload = unload_text_map;

    register_asset_loader("overture:text_map", text_map_loader);
}

REGISTER_SYSTEM(setup_map_asset_loaders, STARTUP);
