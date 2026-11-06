#include "graphics/lights.h"
#include "core/ecs.h"
#include "core/serialize_deserialize.h"
#include "platform/window.h"

REGISTER_COMPONENT(point_light_t);

void* deserialize_point_light_t(serial_component_t comp) {
    point_light_t* light = malloc(sizeof(point_light_t));

    for (size_t i = 0; i < comp.data_count; i++) {
        if (strcmp(comp.data[i].name, "colour") == 0) {
            memcpy(light->colour.raw, comp.data[i].data, sizeof(float[3]));
        }
        if (strcmp(comp.data[i].name, "intensity") == 0) {
            light->intensity = *(float*)comp.data[i].data;
        }
    }

    light->window_id = get_last_window();

    return light;
}
