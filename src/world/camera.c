#include "core/ecs.h"
#include "core/serialize_deserialize.h"
#include "core/systems.h"
#include "math/matrix.h"
#include "platform/window.h"
#include "world/camera.h"

REGISTER_COMPONENT(camera_t);

void* deserialize_camera_t(serial_component_t comp) {
    camera_t* camera = malloc(sizeof(camera_t));
    camera->window_id = get_last_window();

    for (size_t i = 0; i < comp.data_count; i++) {
        if (strcmp(comp.data[i].name, "fov") == 0) {
            camera->fov = *(float*)comp.data[i].data;
        }
        if (strcmp(comp.data[i].name, "aspect_ratio") == 0) {
            camera->aspect_ratio = *(float*)comp.data[i].data;
        }
        if (strcmp(comp.data[i].name, "near") == 0) {
            camera->near = *(float*)comp.data[i].data;
        }
        if (strcmp(comp.data[i].name, "far") == 0) {
            camera->far = *(float*)comp.data[i].data;
        }
    }

    return camera;
}

void update_camera_proj() {
    entity_t* list = FILTER_ENTITIES(camera_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        camera_t* camera = get_comp(list[i], GET_ID(camera_t));

        window_data_t* window = get_window(camera->window_id);

        // TODO: have some sort of control over these
        camera->aspect_ratio = (float)window->context.width / (float)window->context.height;
        mat4_t proj = mat4_perspective_proj(camera->fov, camera->aspect_ratio, camera->near, camera->far);

        window->context.proj = proj;
    }

    free(list);
}

REGISTER_SYSTEM(update_camera_proj, PRE_RENDER);
