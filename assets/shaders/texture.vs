#version 430 core

#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

uniform mat4 world;
uniform mat4 proj;
uniform mat4 view;

void main()
{
   gl_Position = proj * view * world * vec4(aPos, 1.0);
   FragPos = vec3(world * vec4(aPos, 1.0));
   Normal = aNormal;
   UV = aUV;
}
