#version 330 core

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec4 v_FragPosLightSpace;
in mat3 v_TBN;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_ShadowMap;
uniform int       u_UseNormalMap; // 0 = flat normal, 1 = normal map
uniform vec3 u_LightDir;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;

// fog
uniform vec3  u_FogColor;
uniform float u_FogDensity;
uniform int   u_FogEnabled;

#define MAX_POINT_LIGHTS 128

struct PointLight {
    vec3  position;
    vec3  color;
    float intensity;
    float radius;
};

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int        u_NumPointLights;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3  lightDir    = normalize(light.position - fragPos);
    float distance    = length(light.position - fragPos);

    float attenuation = clamp(1.0 - (distance / light.radius), 0.0, 1.0);
    attenuation       = pow(attenuation, 3.0);

    vec3  ambient     = 0.1 * light.color * light.intensity * attenuation;

    float diff        = max(dot(normal, lightDir), 0.0);
    vec3  diffuse     = diff * light.color * light.intensity * attenuation;

    vec3  halfwayDir  = normalize(lightDir + viewDir);
    float spec        = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3  specular    = spec * light.color * light.intensity * attenuation * 0.3;

    return ambient + diffuse + specular;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(u_ShadowMap,
                projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3 texColor = texture(u_Texture, v_TexCoord).rgb;

    // get normal — from normal map or flat surface normal
    vec3 norm;
    if (u_UseNormalMap == 1) {
        // sample normal map, convert from [0,1] to [-1,1]
        vec3 sampledNormal = texture(u_NormalMap, v_TexCoord).rgb;
        sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
        norm = normalize(v_TBN * sampledNormal);
    } else {
        // use the surface normal from TBN (last column)
        norm = normalize(v_TBN[2]);
    }

    vec3 viewDir  = normalize(u_ViewPos - v_FragPos);
    vec3 lightDir = normalize(u_LightDir);

    // ambient
    vec3 ambient  = 0.01 * u_LightColor;

    // directional light
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * u_LightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec      = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular   = 0.3 * spec * u_LightColor;

    // shadow
    float shadow = ShadowCalculation(v_FragPosLightSpace, norm, lightDir);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;

    // point lights
    for (int i = 0; i < u_NumPointLights; i++)
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir) * texColor;

    // fog
    if (u_FogEnabled == 1) {
        float distance  = length(u_ViewPos - v_FragPos);
        float fogFactor = exp(-u_FogDensity * distance);
        fogFactor       = clamp(fogFactor, 0.0, 1.0);
        result          = mix(u_FogColor, result, fogFactor);
    }

    FragColor = vec4(result, 1.0);
}