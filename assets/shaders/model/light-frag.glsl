#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_LIGHTS 4
#define MAX_TEXTURES 1

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 viewPos;

uniform int nDirLights;
uniform int nPointLights;
uniform int nSpotLights;

uniform DirLight dirLights[MAX_LIGHTS];
uniform PointLight pointLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];

uniform sampler2D texture_diffuse[1];
uniform sampler2D texture_specular[1];

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 result = vec3(0.f);

    for (int i = 0; i < nDirLights; ++i) {
        result += calcDirLight(dirLights[i], norm, viewDir);
    }

    for (int i = 0; i < nPointLights; ++i) {
        result += calcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
    }

    for (int i = 0; i < nSpotLights; ++i) {
        result += calcSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir);
    }

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

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient = light.ambient * getDiffuse();
    vec3 diffuse = light.diffuse * calcDiffuse(lightDir, normal) * getDiffuse();
    vec3 specular = light.specular * calcSpecular(lightDir, viewDir, normal) 
        * getSpecular();

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float distance = length(light.position - fragPos);
    float attenuation = calcAttenuation(distance, light.constant, light.linear
            , light.quadratic);

    vec3 lightDir = normalize(light.position - fragPos);
    vec3 ambient = light.ambient * getDiffuse();
    vec3 diffuse = light.diffuse * calcDiffuse(lightDir, normal) * getDiffuse();
    vec3 specular = light.specular * calcSpecular(lightDir, viewDir, normal) 
        * getSpecular();

    return attenuation * (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = calcAttenuation(distance, light.constant, light.linear
            , light.quadratic);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * getDiffuse();
    vec3 diffuse = light.diffuse * calcDiffuse(lightDir, normal) * getDiffuse();
    vec3 specular = light.specular * calcSpecular(lightDir, viewDir, normal) 
        * getSpecular();

    return attenuation * intensity * (ambient + diffuse + specular);
}

