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
    vec4 FragPosLightSpace;
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
uniform sampler2D shadowMap;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main() {
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 result = vec3(0.f);

    float shadow = CalculateShadow(fs_in.FragPosLightSpace, norm, normalize(-dirLights[0].direction));

    for (int i = 0; i < nDirLights; ++i) {
        result += calcDirLight(dirLights[i], norm, viewDir, shadow);
    }

    for (int i = 0; i < nPointLights; ++i) {
        result += calcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir, shadow);
    }

    for (int i = 0; i < nSpotLights; ++i) {
        result += calcSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir, shadow);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.f);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse[0], fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse[0], fs_in.TexCoords));
    vec3 specular = light.specular * spec * texture(texture_specular[0], fs_in.TexCoords).rrr;

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.f);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear 
            * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse[0], fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse[0], fs_in.TexCoords));
    vec3 specular = light.specular * spec * texture(texture_specular[0], fs_in.TexCoords).rrr;

    return attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.f);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear 
            * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse[0], fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse[0], fs_in.TexCoords));
    vec3 specular = light.specular * spec * texture(texture_specular[0], fs_in.TexCoords).rrr;

    return attenuation * intensity * (ambient + (1.0 - shadow) * (diffuse + specular));
}

float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.00005);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}
