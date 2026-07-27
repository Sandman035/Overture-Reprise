#version 430 core

#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

layout(bindless_sampler) uniform sampler2D diffuse;
layout(bindless_sampler) uniform sampler2D ao;

void main()
{
    vec3 objectColor = texture(diffuse, UV).xyz * texture(ao, UV).x;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(2.0) - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    vec3 result = (vec3(0.1, 0.1, 0.1) + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
