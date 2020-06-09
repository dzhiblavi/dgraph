#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;

uniform vec3 LightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;
out vec3 lightPos;
out vec2 texCoords;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    normal = mat3(transpose(inverse(view * model))) * Normal;
    fragPos = vec3(view * model * vec4(position, 1.0f));
    lightPos = vec3(view * vec4(LightPos, 1.f));
    texCoords = TexCoords;
}
