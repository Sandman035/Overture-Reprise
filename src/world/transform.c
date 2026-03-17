#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include "graphics/render_obj.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "platform/window.h"
#include "world/camera.h"
#include "world/transform.h"

REGISTER_COMPONENT(transform_t);

void update_render_obj_transform() {
    entity_t** list = FILTER_ENTITIES(transform_t, render_object_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        TRACE("Updating render obj %p transform matrix", ent_ptr);

        render_object_t* obj = get_comp(*ent_ptr, GET_ID(render_object_t));
        transform_t* transform = get_comp(*ent_ptr, GET_ID(transform_t));

        mat4_t world = mat4_identity();
        world = scale_mat4(world, transform->scale);
        world = rotate_mat4(world, transform->rot);
        world = translate_mat4(world, transform->pos);
        obj->world_transform = world;

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update_render_obj_transform, PRE_RENDER);

void update_camera_transform() {
    entity_t** list = FILTER_ENTITIES(transform_t, camera_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        TRACE("Updating camera %p transform matrix", ent_ptr);

        camera_t* camera = get_comp(*ent_ptr, GET_ID(camera_t));
        transform_t* transform = get_comp(*ent_ptr, GET_ID(transform_t));

        mat4_t view = mat4_identity();
        //view = scale_mat4(view, vec3_scale(transform->scale, -1.0)); // this isn't scaled but inverted
        view = translate_mat4(view, vec3_scale(transform->pos, -1.0));
        view = rotate_mat4(view, vec3_scale(transform->rot, -1.0));
        get_window(camera->window_id)->context.view = view;

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update_camera_transform, PRE_RENDER);
