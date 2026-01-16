#ifndef OVERTURE_OPENGL
#define OVERTURE_OPENGL

#include <stddef.h>
#include <stdint.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


void cleanup_opengl();

void setup_gl_window();
void begin_gl_window_render();

void resize_gl_viewport(uint32_t width, uint32_t height);

typedef uint32_t program_t;

typedef enum {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
} shader_type_t;

program_t create_program();
void destroy_program(program_t program);
void add_shader(program_t program, const char* shader_source, shader_type_t shader_type);

// NOTE: idk if ill keep this tho it does save a bit of work tho its hard to document
// TODO: maybe add a way to check if uniform exists and only set it if it does
#define SET_UNIFORM(type, program, name, ...) \
    glUniform ## type(glGetUniformLocation(program, name), __VA_ARGS__);

typedef struct {
    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
    uint32_t attrib_count;
} vertex_buffer_t;

vertex_buffer_t create_vertex_buffer(size_t size, void* data);
void destroy_vertex_buffer(vertex_buffer_t* vertex_buffer);
void add_attrib(vertex_buffer_t* vertex_buffer, uint32_t size, GLenum type, size_t stride, size_t offset);
void add_index_buffer(vertex_buffer_t* vertex_buffer, size_t size, void* data);

#endif
