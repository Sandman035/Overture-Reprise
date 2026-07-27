#include "render_obj.h"
#include "assets/asset_manager.h"
#include "graphics/mesh_asset.h"
#include "graphics/opengl.h"
#include "graphics/texture_asset.h"
#include "math/matrix.h"
#include "platform/window.h"
#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdint.h>
#include <stdlib.h>

REGISTER_COMPONENT(render_object_t);
REGISTER_COMPONENT(z_pre_pass_t);
REGISTER_COMPONENT(transparent_material_t);
REGISTER_COMPONENT(textures_t);

object_renderer_context create_object_renderer_context(uint32_t width, uint32_t height) {
    object_renderer_context context;
    
    create_object_renderer_framebuffers(&context, width, height);

    context.view = mat4_identity();
    context.proj = mat4_identity();

    context.width = width;
    context.height = height;

    context.opaque_z_pre.n = 0;
    context.opaque_no_z.n = 0;
    context.transparent.n = 0;

    context.opaque_z_pre.entities = NULL;
    context.opaque_no_z.entities = NULL;
    context.transparent.entities = NULL;

    context.opaque_z_pre.allocated = 0;
    context.opaque_no_z.allocated = 0;
    context.transparent.allocated = 0;

    return context;
}

void rebuild_object_renderer_framebuffers(object_renderer_context* context, uint32_t width, uint32_t height) {
    // NOTE: idk if the framebuffers need to be destroyed or only the texture and render buffer
    destroy_object_renderer_framebuffers(context);

    create_object_renderer_framebuffers(context, width, height);

    context->width = width;
    context->height = height;
}

void create_object_renderer_framebuffers(object_renderer_context* context, uint32_t width, uint32_t height) {
    uint16_t samples = 4;
    // opaque pass
    glGenFramebuffers(1, &context->opaque_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, context->opaque_fbo);

    glGenTextures(1, &context->opaque_color_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, context->opaque_color_texture);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, context->opaque_color_texture, 0);

    glGenRenderbuffers(1, &context->opaque_depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, context->opaque_depth_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, context->opaque_depth_rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("Opaque pass framebuffer is not complete.");
    }

    TRACE("Created opaque pass framebuffer.");

    glGenFramebuffers(1, &context->transparent_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, context->transparent_fbo);

    glGenTextures(1, &context->transparent_color_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, context->transparent_color_texture);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, context->transparent_color_texture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("Transparent pass framebuffer is not complete.");
    }

    TRACE("Created transparent pass framebuffer.");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void destroy_object_renderer_framebuffers(object_renderer_context* context) {
    glDeleteRenderbuffers(1, &context->opaque_depth_rbo);
    glDeleteTextures(1, &context->opaque_color_texture);
    glDeleteFramebuffers(1, &context->opaque_fbo);
    glDeleteTextures(1, &context->transparent_color_texture);
    glDeleteFramebuffers(1, &context->transparent_fbo);
}

void clear_object_rederer_framebuffers(object_renderer_context* context) {
    glBindFramebuffer(GL_FRAMEBUFFER, context->opaque_fbo);
    glDepthMask(GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: more
}

void sort_render_objs() {
    // Potentially this sorting could be used to create an auto instancer, as in every render obj that shares the same shaders and buffers
    // would be combined into a singular draw call via instancing

    TRACE("Sorting render objects to windows");

    entity_t* list = FILTER_ENTITIES_EXCLUDING((render_object_t, z_pre_pass_t), (transparent_material_t));

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        window->context.opaque_z_pre.n++;
    }

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        render_queue_t* queue = &window->context.opaque_z_pre;

        if (!queue->allocated) {
            queue->entities = malloc(queue->n * sizeof(entity_t));
            queue->allocated = 1;

            queue->n = 0; // this is so we can use this as the idx
        }

        // TODO: have some sort of checks like frustum culling etc.

        queue->entities[queue->n] = list[i];
        queue->n++;
    }

    free(list);

    list = FILTER_ENTITIES_EXCLUDING((render_object_t), (z_pre_pass_t, transparent_material_t));

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        window->context.opaque_no_z.n++;
    }

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        render_queue_t* queue = &window->context.opaque_no_z;

        if (!queue->allocated) {
            queue->entities = malloc(queue->n * sizeof(entity_t));
            queue->allocated = 1;

            queue->n = 0; // this is so we can use this as the idx
        }

        queue->entities[queue->n] = list[i];
        queue->n++;
    }

    free(list);

    list = FILTER_ENTITIES(render_object_t, transparent_material_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        window->context.transparent.n++;
    }

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        render_queue_t* queue = &window->context.transparent;

        if (!queue->allocated) {
            queue->entities = malloc(queue->n * sizeof(entity_t));
            queue->allocated = 1;

            queue->n = 0; // this is so we can use this as the idx
        }

        queue->entities[queue->n] = list[i];
        queue->n++;
    }

    free(list);
}

REGISTER_SYSTEM(sort_render_objs, PRE_RENDER);

void clear_render_queues(object_renderer_context* context) {
    context->opaque_z_pre.n = 0;
    context->opaque_no_z.n = 0;
    context->transparent.n = 0;

    free(context->opaque_z_pre.entities);
    free(context->opaque_no_z.entities);
    free(context->transparent.entities);

    context->opaque_z_pre.entities = NULL;
    context->opaque_no_z.entities = NULL;
    context->transparent.entities = NULL;

    context->opaque_z_pre.allocated = 0;
    context->opaque_no_z.allocated = 0;
    context->transparent.allocated = 0;
}

void render_queues(object_renderer_context* context) {
    // TODO: get window info
    TRACE("Start object renderer.");

    /******************/
    /* Depth Pre-pass */
    /******************/
    // Renders all opaque objects to the depth buffer

    TRACE("Start depth pre-pass.");

    entity_t* list = context->opaque_z_pre.entities;

    for (uint64_t i = 0; i < context->opaque_z_pre.n; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        z_pre_pass_t* z_pre_pass = get_comp(list[i], GET_ID(z_pre_pass_t));

        mesh_asset_t* mesh = get_asset(obj->mesh);

        if (mesh == NULL) {
            continue;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, context->opaque_fbo);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        glDepthFunc(GL_LESS);

        glUseProgram(z_pre_pass->program);

        SET_UNIFORM(Matrix4fv, z_pre_pass->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, z_pre_pass->program, "view", 1, GL_TRUE, &context->view.m00);
        SET_UNIFORM(Matrix4fv, z_pre_pass->program, "proj", 1, GL_TRUE, &context->proj.m00);

        glBindVertexArray(mesh->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, mesh->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0); // the last zero might be a problem it should be a pointer somewhere
    }

    /***************/
    /* Opaque pass */
    /***************/
    // Renders all opaque objects to the color buffer with the depth check set to equal to to minimize lighting and shader calculations

    TRACE("Start opaque pass.");

    for (uint64_t i = 0; i < context->opaque_z_pre.n; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        textures_t* textures = get_comp(list[i], GET_ID(textures_t));

        mesh_asset_t* mesh = get_asset(obj->mesh);

        if (mesh == NULL) {
            continue;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, context->opaque_fbo);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_EQUAL);
        glDepthMask(GL_FALSE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);

        glUseProgram(obj->program);

        SET_UNIFORM(Matrix4fv, obj->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "view", 1, GL_TRUE, &context->view.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "proj", 1, GL_TRUE, &context->proj.m00);

        if (textures != NULL) {
            for (uint32_t i = 0; i < textures->count; i++) {
                texture_asset_t* texture = get_asset(textures->textures[i]);
                if (texture != NULL) {
                    DEBUG("%s, %ld", textures->names[i], textures->textures[i]);
                    SET_UNIFORM(Handleui64ARB, obj->program, textures->names[i], texture->texture.texture_handle_arb);
                }
            }
        }

        glBindVertexArray(mesh->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, mesh->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0); // the last zero might be a problem it should be a pointer somewhere
    }

    TRACE("Render non-z_pre_pass objects.");

    list = context->opaque_no_z.entities;

    for (uint64_t i = 0; i < context->opaque_no_z.n; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        textures_t* textures = get_comp(list[i], GET_ID(textures_t));

        mesh_asset_t* mesh = get_asset(obj->mesh);

        if (mesh == NULL) {
            continue;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, context->opaque_fbo);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glUseProgram(obj->program);

        SET_UNIFORM(Matrix4fv, obj->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "view", 1, GL_TRUE, &context->view.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "proj", 1, GL_TRUE, &context->proj.m00);

        if (textures != NULL) {
            for (uint32_t i = 0; i < textures->count; i++) {
                texture_asset_t* texture = get_asset(textures->textures[i]);
                if (texture != NULL) {
                    SET_UNIFORM(Handleui64ARB, obj->program, textures->names[i], texture->texture.texture_handle_arb);
                }
            }
        }

        glBindVertexArray(mesh->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, mesh->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0);
    }

    /********************/
    /* Transparent pass */
    /********************/

    TRACE("Start transparent pass.");

    list = context->transparent.entities;

    for (uint64_t i = 0; i < context->transparent.n; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        textures_t* textures = get_comp(list[i], GET_ID(textures_t));

        mesh_asset_t* mesh = get_asset(obj->mesh);

        if (!mesh) {
            continue;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, context->transparent_fbo);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glUseProgram(obj->program);

        SET_UNIFORM(Matrix4fv, obj->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "view", 1, GL_TRUE, &context->view.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "proj", 1, GL_TRUE, &context->proj.m00);

        if (textures != NULL) {
            for (uint32_t i = 0; i < textures->count; i++) {
                texture_asset_t* texture = get_asset(textures->textures[i]);
                if (texture != NULL) {
                    SET_UNIFORM(Handleui64ARB, obj->program, textures->names[i], texture->texture.texture_handle_arb);
                }
            }
        }

        glBindVertexArray(mesh->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, mesh->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0);
    }

    // TODO: actually implement a proper OIT renderpass

    TRACE("Complete object renderer.");
}

// TODO: cleanup render obj and render object context
