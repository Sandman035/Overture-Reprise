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
            char func_name[42] = "deserialize_";
            strcat(func_name, tree.ents[i].comps[j].name);
            void*(*func)(serial_component_t) = dlsym(dl, func_name);

            void* data = func(tree.ents[i].comps[j]);

            char add_func_name[34] = "add_";
            strcat(add_func_name, tree.ents[i].comps[j].name);
            void(*add_func)(entity_t, void*) = dlsym(dl, add_func_name);

            add_func(ent, data);
            
            free(data);
        }
    }
    //dlclose(dl);
}
