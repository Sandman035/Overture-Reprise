#include "render_obj.h"
#include "graphics/opengl.h"
#include "math/matrix.h"
#include "platform/window.h"
#include "core/ecs.h"
#include "core/log.h"
#include "core/systems.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

REGISTER_COMPONENT(render_object_t);
REGISTER_COMPONENT(z_pre_pass_t);
REGISTER_COMPONENT(transparent_material_t);

object_renderer_context create_object_renderer_context(uint32_t width, uint32_t height) {
    object_renderer_context context;
    
    create_object_renderer_framebuffers(&context, width, height);

    context.view = mat4_identity();
    context.proj = mat4_identity();

    context.width = width;
    context.height = height;

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

    // TODO: figure out optimal texture settings
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    // transparent pass
    glGenFramebuffers(1, &context->transparent_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, context->transparent_fbo);

    glGenTextures(1, &context->transparent_color_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, context->transparent_color_texture);

    // TODO: figure out optimal texture settings
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    
    // attach it to currently bound framebuffer object
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

void render_obj() {
    TRACE("Start object renderer.");

    /******************/
    /* Depth Pre-pass */
    /******************/
    // Renders all opaque objects to the depth buffer

    // TODO: have some sort of system to keep having to bind framebuffers over and over again and same with settign window as context

    TRACE("Start depth pre-pass.");

    entity_t* list = FILTER_ENTITIES(render_object_t, z_pre_pass_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));
        z_pre_pass_t* z_pre_pass = get_comp(list[i], GET_ID(z_pre_pass_t));

        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        glfwMakeContextCurrent(window->window); // temp

        glBindFramebuffer(GL_FRAMEBUFFER, window->context.opaque_fbo);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        glDepthFunc(GL_LESS);

        glUseProgram(z_pre_pass->program);

        SET_UNIFORM(Matrix4fv, z_pre_pass->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, z_pre_pass->program, "view", 1, GL_TRUE, &window->context.view.m00);
        SET_UNIFORM(Matrix4fv, z_pre_pass->program, "proj", 1, GL_TRUE, &window->context.proj.m00);

        glBindVertexArray(obj->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, obj->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0); // the last zero might be a problem it should be a pointer somewhere
    }

    /***************/
    /* Opaque pass */
    /***************/
    // Renders all opaque objects to the color buffer with the depth check set to equal to to minimize lighting and shader calculations

    TRACE("Start opaque pass.");

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));

        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        glfwMakeContextCurrent(window->window); // temp

        glBindFramebuffer(GL_FRAMEBUFFER, window->context.opaque_fbo);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_EQUAL);
        glDepthMask(GL_FALSE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glUseProgram(obj->program);

        SET_UNIFORM(Matrix4fv, obj->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "view", 1, GL_TRUE, &window->context.view.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "proj", 1, GL_TRUE, &window->context.proj.m00);

        glBindVertexArray(obj->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, obj->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0); // the last zero might be a problem it should be a pointer somewhere
    }

    free(list);

    /********************/
    /* Transparent pass */
    /********************/

    TRACE("Start transparent pass.");

    list = FILTER_ENTITIES(render_object_t, transparent_material_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        render_object_t* obj = get_comp(list[i], GET_ID(render_object_t));

        window_data_t* window = get_window(obj->window_id);

        if (window == NULL) {
            WARN("Window %ld doesn't exist skipping render obj %p.", obj->window_id, obj);
            continue;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, window->context.transparent_fbo);

        glUseProgram(obj->program);

        SET_UNIFORM(Matrix4fv, obj->program, "world", 1, GL_TRUE, &obj->world_transform.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "view", 1, GL_TRUE, &window->context.view.m00);
        SET_UNIFORM(Matrix4fv, obj->program, "proj", 1, GL_TRUE, &window->context.proj.m00);

        glBindVertexArray(obj->vertex_buffer.VAO);
        glDrawElements(GL_TRIANGLES, obj->vertex_buffer.indices_count, GL_UNSIGNED_INT, 0); // the last zero might be a problem it should be a pointer somewhere
    }

    // TODO: actually implement a proper OIT renderpass

    free(list);

    TRACE("Complete object renderer.");
}

REGISTER_SYSTEM_FRONT(render_obj, RENDER);

// TODO: cleanup render obj and render object context
