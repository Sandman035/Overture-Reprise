#include "graphics/opengl.h"
#include "core/log.h"
#include <stddef.h>

void setup_gl_window() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ERROR("Failed to create opengl context.");
    }
    TRACE("Created opengl context.");
}

// NOTE: this won't be necessary after obj rendering is implemented
void begin_gl_window_render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

vertex_buffer_t create_vertex_buffer(size_t size, void* data) {
    vertex_buffer_t buffer;

    buffer.attrib_count = 0;
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

void add_attrib(vertex_buffer_t* vertex_buffer, uint32_t size, GLenum type, size_t stride, size_t offset) {
    glBindVertexArray(vertex_buffer->VAO);
    glVertexAttribPointer(vertex_buffer->attrib_count, size, type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(vertex_buffer->attrib_count);
    vertex_buffer->attrib_count++;
}

void add_index_buffer(vertex_buffer_t* vertex_buffer, size_t size, void* data) {
    glBindVertexArray(vertex_buffer->VAO);
    glGenBuffers(1, &vertex_buffer->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void cleanup_opengl() {
}
