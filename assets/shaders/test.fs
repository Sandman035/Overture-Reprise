#version 430 core

#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform vec3 viewPos;

layout(bindless_sampler) uniform sampler2D diffuse;
layout(bindless_sampler) uniform sampler2D ao;
layout(bindless_sampler) uniform sampler2D specular;

struct PointLight {
    vec3 pos;
    vec3 colour;
    float intensity;
};

layout(std430, binding = 0) buffer PointLightBuffer {
    PointLight lights[];
};

void main()
{
    vec3 objectColor = texture(diffuse, UV).xyz * texture(ao, UV).x;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffuse_colour = vec3(0.0);
    vec3 specular_colour = vec3(0.0);

    for (int i = 0; i < lights.length(); i++)
    {
        PointLight light = lights[i];

        vec3 lightDir = normalize(light.pos - FragPos);

        float diff = max(dot(norm, lightDir), 0.0);
        diffuse_colour += diff * light.colour;

        // fix this
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 64);
        specular_colour += spec * texture(specular, UV).x * light.colour;
    }

    vec3 result = (vec3(0.1, 0.1, 0.1) + diffuse_colour) * objectColor + specular_colour;
    FragColor = vec4(result, 1.0);
}
