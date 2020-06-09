#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

out vec4 color;

void main() {
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * objectColor * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 1.f;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0f);
}
