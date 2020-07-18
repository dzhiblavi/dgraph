#version 330 core
out vec4 FragColor;

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 viewPos;
uniform float far_plane;
uniform PointLight pointLight;
uniform sampler2D texture_diffuse[1];
uniform sampler2D texture_specular[1];
uniform samplerCube shadowMap;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir
        , float shadow);
float calculateShadow(vec3 fragPos, vec3 lightPos);

void main() {
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float shadow = calculateShadow(fs_in.FragPos, pointLight.position);
    vec3 result = vec3(0.f);

    result = calcPointLight(pointLight, norm, fs_in.FragPos, viewDir, shadow);
    FragColor = vec4(result, 1.0);
}

float calcSpecular(vec3 lightDir, vec3 viewDir, vec3 normal) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    return pow(max(dot(normal, halfwayDir), 0.0), 64.f);
}

float calcDiffuse(vec3 lightDir, vec3 normal) {
    return max(0.0f, dot(lightDir, normal));
}

float calcAttenuation(float distance, float constant, float linear, float quadratic) {
    return 1.0 / (constant + linear
            * distance + quadratic * (distance * distance));
}

vec3 getDiffuse() {
    return vec3(texture(texture_diffuse[0], fs_in.TexCoords));
}

vec3 getSpecular() {
    return texture(texture_specular[0], fs_in.TexCoords).rrr;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir
        , float shadow) {
    float distance = length(light.position - fragPos);
    float attenuation = calcAttenuation(distance, light.constant, light.linear
            , light.quadratic);

    vec3 lightDir = normalize(light.position - fragPos);
    vec3 ambient = light.ambient * getDiffuse();
    vec3 diffuse = light.diffuse * calcDiffuse(lightDir, normal) * getDiffuse();
    vec3 specular = light.specular * calcSpecular(lightDir, viewDir, normal) 
        * getSpecular();

    return attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 sampleOffsetDirections[20] = vec3[](
    vec3( 0,  0,  0), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calculateShadow(vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 1;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    for (int i = 0; i < samples; ++i) {
        float closestDepth = texture(shadowMap, fragToLight
                + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);
    return shadow;
}

