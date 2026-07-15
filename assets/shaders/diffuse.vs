#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 world;
uniform mat4 proj;
uniform mat4 view;

void main()
{
   gl_Position = proj * view * world * vec4(aPos, 1.0);
   FragPos = vec3(world * vec4(aPos, 1.0));
   Normal = aNormal;
}
