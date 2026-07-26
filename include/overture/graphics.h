#ifndef OVERTURE_GRAPHICS_H
#define OVERTURE_GRAPHICS_H

#include <stddef.h>
#include <stdint.h>
#include <glad/glad.h>

#include <overture/math.h>
#include <overture/assets.h>

/// Shader program.
typedef uint32_t program_t;

/// Types of shaders supported.
typedef enum {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
} shader_type_t;

typedef struct {
    shader_type_t type;
    uint32_t shader_id;
} shader_asset_t;

typedef struct {
    shader_type_t type;
} glsl_shader_loader_args_t;

/// Create a shader program and return it.
program_t create_program();

/// Destroy a shader program.
void destroy_program(program_t program);

/// Add a shader to the program from a array of characters containing the shader source.
void add_shader(program_t program, const char* shader_source, shader_type_t shader_type);

/// Add a shader to the program from a previously loaded shader asset.
void add_shader_asset(program_t program, asset_handle_t shader_handle);

/// Convoluted way of setting a uniform.
#define SET_UNIFORM(type, program, name, ...) \
    glUniform ## type(glGetUniformLocation(program, name), __VA_ARGS__);

/// Vertex Buffer Data
typedef struct {
    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
    uint32_t next_attrib;
    uint64_t indices_count;
} vertex_buffer_t;

/// Create a vertex buffer based on verticies.
vertex_buffer_t create_vertex_buffer(size_t size, void* data);

/// Destroy vertex buffer data.
void destroy_vertex_buffer(vertex_buffer_t* vertex_buffer);

/// Add an attribute to the vertex buffer.
void add_attrib(vertex_buffer_t* vertex_buffer, uint32_t size, GLenum type, size_t stride, size_t offset);

/// Add an attribute to the vertex buffer at a specific index.
void instert_attrib(vertex_buffer_t* vertex_buffer, uint32_t index, uint32_t size, GLenum type, size_t stride, size_t offset);

/// Add an index buffer to the vertex buffer.
void add_index_buffer(vertex_buffer_t* vertex_buffer, size_t size, void* data);

/// Render object component
typedef struct {
    uint64_t window_id;
    program_t program;
    asset_handle_t mesh;
    mat4_t world_transform;
} render_object_t;

///  Z pre-pass component
typedef struct {
    program_t program;
} z_pre_pass_t;

/// Transparent material label component
typedef struct {} transparent_material_t;

typedef struct {
    vec3_t pos;
    vec3_t norm;
    vec2_t tex;
} vertex_t;

typedef struct {
    vertex_buffer_t vertex_buffer;
} mesh_asset_t;

#endif
