#include "core/ecs.h"
#include "core/systems.h"
#include "graphics/render_obj.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "platform/window.h"
#include "world/camera.h"
#include "world/transform.h"

REGISTER_COMPONENT(transform_t);

void update_render_obj_transform() {
    entity_t*list = FILTER_ENTITIES(transform_t, render_object_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        transform_t* transform = get_comp(list[i], GET_ID(transform_t));

        mat4_t world = mat4_identity();
        world = scale_mat4(world, transform->scale);
        world = rotate_mat4(world, transform->rot);
        world = translate_mat4(world, transform->pos);
        obj->world_transform = world;
    }

    free(list);
}

REGISTER_SYSTEM(update_render_obj_transform, PRE_RENDER);

void update_camera_transform() {
    entity_t* list = FILTER_ENTITIES(transform_t, camera_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        camera_t* camera = get_comp(list[i], GET_ID(camera_t));
        transform_t* transform = get_comp(list[i], GET_ID(transform_t));

        mat4_t view = mat4_identity();
        //view = scale_mat4(view, vec3_scale(transform->scale, -1.0)); // this isn't scaled but inverted
        view = translate_mat4(view, vec3_scale(transform->pos, -1.0));
        view = rotate_mat4(view, vec3_scale(transform->rot, -1.0));
        get_window(camera->window_id)->context.view = view;
    }

    free(list);
}

REGISTER_SYSTEM(update_camera_transform, PRE_RENDER);
