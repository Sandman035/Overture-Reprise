#include "core/serialize_deserialize.h"
#include "core/ecs.h"
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// rename???
void actualize_scene_tree(serial_scene_tree_t tree) {
    // maybe do this only once
    void* dl = dlopen(NULL, 0);

    for (size_t i = 0; i < tree.ent_count; i++) {
        entity_t ent = create_entity();

        for (size_t j = 0; j < tree.ents[i].comp_count; j++) {
            const char* des_string = "deserialize_";
            char func_name[42] = {0};
            snprintf(func_name, sizeof(func_name), "%s%s", des_string, tree.ents[i].comps[j].name);
            void*(*func)(serial_component_t) = dlsym(dl, func_name);

            void* data = func(tree.ents[i].comps[j]);

            const char* add_string = "add_";
            char add_func_name[34] = {0};
            snprintf(add_func_name, sizeof(add_func_name), "%s%s", add_string, tree.ents[i].comps[j].name);
            void(*add_func)(entity_t, void*) = dlsym(dl, add_func_name);

            add_func(ent, data);
            
            free(data);
        }
    }
    //dlclose(dl);
}
