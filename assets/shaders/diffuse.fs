#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

void main()
{
    vec3 objectColor = vec3(0.9215686274509803, 0.20392156862745098, 0.20392156862745098);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0) - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    vec3 result = (vec3(0.1, 0.1, 0.1) + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
