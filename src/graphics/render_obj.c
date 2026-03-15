#include "render_obj.h"
#include "graphics/opengl.h"
#include "math/matrix.h"
#include "platform/window.h"
#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include <GL/gl.h>
#include <glad/glad.h>

REGISTER_COMPONENT(opaque_render_object_t);
REGISTER_COMPONENT(transparent_render_object_t);

object_renderer_context create_object_renderer_context(uint32_t width, uint32_t height) {
    object_renderer_context context;
    context.view = mat4_identity();
    context.proj = mat4_identity();

    return context;
}

void clear_object_rederer_framebuffers(object_renderer_context* context) {
}

void render_obj() {
    TRACE("Start object renderer.");

    uint64_t current_win = 0;
    uint8_t started = 0;

    entity_t** list = FILTER_ENTITIES(opaque_render_object_t);

    entity_t** ent_ptr = list;
    TRACE("Start opaque pass.");

    ent_ptr = list;
    while (*ent_ptr != NULL) {
        opaque_render_object_t* obj = get_comp(*ent_ptr, GET_ID(opaque_render_object_t));

        window_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        if (!started || current_win != obj->window_id) {
            // temp
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            started = 1;
        }

        current_win = obj->window_id;

        glUseProgram(obj->color_program);

        SET_UNIFORM(Matrix4fv, obj->color_program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, obj->color_program, "view", 1, GL_TRUE, &window->context.view.m00);
        SET_UNIFORM(Matrix4fv, obj->color_program, "proj", 1, GL_TRUE, &window->context.proj.m00);

        glBindVertexArray(obj->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // the last zero might be a problem it should be a pointer somewhere

        ent_ptr++;
    }

    started = 0;

    free(list);

    TRACE("Complete object renderer.");
}

REGISTER_SYSTEM_FRONT(render_obj, RENDER);

// TODO: cleanup render obj and render object context
