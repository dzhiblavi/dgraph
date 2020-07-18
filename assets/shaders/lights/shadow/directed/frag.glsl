#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform sampler2D texture_diffuse[1];
uniform sampler2D texture_specular[1];
uniform sampler2D shadowMap;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main() {
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float shadow = calculateShadow(fs_in.FragPosLightSpace, norm
            , normalize(-dirLight.direction));
    vec3 result = vec3(0.f);

    result = calcDirLight(dirLight, norm, viewDir, shadow);
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

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * getDiffuse();
    vec3 diffuse = light.diffuse * calcDiffuse(lightDir, normal) * getDiffuse();
    vec3 specular = light.specular * calcSpecular(lightDir, viewDir, normal)
        * getSpecular();

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.00005);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) 
                    * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}

