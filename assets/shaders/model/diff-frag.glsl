#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse[1];

void main() {
    FragColor = vec4(vec3(texture(texture_diffuse[0], TexCoords)), 1.f);
}
