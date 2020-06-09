#version 330 core

out vec4 color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPos;
in vec3 normal;
in vec3 lightPos;
in vec2 texCoords;

uniform Material material;
uniform Light light;

void main() {
    vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoords).rgb;

    float specularStrength = 1.f;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * texture(material.specular, texCoords).rrr;

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    		    light.quadratic * (distance * distance));

    vec3 result = attenuation * (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
}
