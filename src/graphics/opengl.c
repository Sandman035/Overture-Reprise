#include "graphics/opengl.h"
#include "assets/asset_manager.h"
#include "core/log.h"
#include "graphics/shader_asset.h"
#include <stddef.h>
#include <stdint.h>

void APIENTRY gl_debug_callback(
    GLenum source, 
    GLenum type, 
    unsigned int id, 
    GLenum severity, 
    GLsizei length, 
    const char *message, 
    const void *userParam
) {
    // TODO: more descriptive messages
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            ERROR("%s", message);
            break;
        };
        case GL_DEBUG_SEVERITY_MEDIUM: {
            WARN("%s", message);
            break;
        };
        case GL_DEBUG_SEVERITY_LOW: {
            INFO("%s", message);
            break;
        };
        case GL_DEBUG_SEVERITY_NOTIFICATION: {
            TRACE("%s", message);
            break;
        };
    }
}

void setup_gl_window() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ERROR("Failed to create opengl context.");
    }
    TRACE("Created opengl context.");
}

void setup_gl_window_debug_callback() {
    int flags; 
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(gl_debug_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }
}

void resize_gl_viewport(uint32_t width, uint32_t height) {
    glViewport(0, 0, width, height);
}

program_t create_program() {
    TRACE("Created shader program.");
    return glCreateProgram();
}

void destroy_program(program_t program) {
    glDeleteProgram(program);
    TRACE("Destroyed shader program.");
}

void add_shader(program_t program, const char* shader_source, shader_type_t shader_type) {
    uint32_t shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int32_t success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        WARN("Shader compilation failed: %s.", info_log);
    }

    TRACE("Created shader.");

    glAttachShader(program, shader);
    glLinkProgram(program); // TODO: figure out this works and its not a problem
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        WARN("Shader linking failed: %s.", info_log);
    }

    TRACE("Linked shader.");
    
    glDeleteShader(shader); // TODO: maybe have a seperate shader type so if a different program needs a shader then no need to recompile
}

void add_shader_asset(program_t program, asset_handle_t shader_handle) {
    shader_asset_t* shader_asset = get_asset(shader_handle);

    if (shader_asset == NULL) {
        WARN("Failed to add shader to program.");
        return;
    }

    glAttachShader(program, shader_asset->shader_id);
    glLinkProgram(program); // TODO: figure out this works and its not a problem
    
    int32_t success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        WARN("Shader linking failed: %s.", info_log);
        return;
    }

    TRACE("Linked shader.");
}

vertex_buffer_t create_vertex_buffer(size_t size, void* data) {
    vertex_buffer_t buffer;

    buffer.next_attrib = 0;
    buffer.EBO = 0;

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);

    glBindVertexArray(buffer.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    TRACE("Created vertex buffer.");

    return buffer;
}

void destroy_vertex_buffer(vertex_buffer_t* vertex_buffer) {
    glDeleteVertexArrays(1, &vertex_buffer->VAO);
    glDeleteBuffers(1, &vertex_buffer->VBO);

    if (vertex_buffer->EBO != 0) {
        glDeleteBuffers(1, &vertex_buffer->EBO);
    }
    TRACE("Destroyed vertex buffer.");
}

// TODO: Maube create an attrib descriptor object, that stores a list of attrid contructor instructions, so later a "default" one can be applied automatically
void add_attrib(vertex_buffer_t* vertex_buffer, uint32_t size, GLenum type, size_t stride, size_t offset) {
    glBindVertexArray(vertex_buffer->VAO);
    glVertexAttribPointer(vertex_buffer->next_attrib, size, type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(vertex_buffer->next_attrib);
    vertex_buffer->next_attrib++;
}

void instert_attrib(vertex_buffer_t* vertex_buffer, uint32_t index, uint32_t size, GLenum type, size_t stride, size_t offset) {
    glBindVertexArray(vertex_buffer->VAO);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(index);

    if (vertex_buffer->next_attrib <= index) {
        vertex_buffer->next_attrib = index + 1;
    }
}

void add_index_buffer(vertex_buffer_t* vertex_buffer, size_t size, void* data) {
    glBindVertexArray(vertex_buffer->VAO);
    glGenBuffers(1, &vertex_buffer->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    vertex_buffer->indices_count = size / sizeof(uint32_t);
}

texture_t create_texture(int32_t width, int32_t height, const void* data) {
    texture_t texture;

    glGenTextures(1, &texture.texture);
    glBindTexture(GL_TEXTURE_2D, texture.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // TODO: different format handling
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    texture.texture_handle_arb = glGetTextureHandleARB(texture.texture);

    // TODO: handle texture arb handle error

    // NOTE: idk if I should do this, maybe I shouldn't, but its temp for now
    glMakeTextureHandleResidentARB(texture.texture_handle_arb);

    return texture;
}

void cleanup_opengl() {
}
