#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "math/matrix.h"
#include "platform/window.h"
#include "world/camera.h"

REGISTER_COMPONENT(camera_t);

void update_camera_proj() {
    entity_t** list = FILTER_ENTITIES(camera_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        camera_t* camera = get_comp(*ent_ptr, GET_ID(camera_t));

        // TODO: have some sort of control over these, like window resize etc.
        mat4_t proj = mat4_perspective_proj(camera->fov, camera->aspect_ratio, camera->near, camera->far);

        get_window(camera->window_id)->context.proj = proj;

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update_camera_proj, PRE_RENDER);
