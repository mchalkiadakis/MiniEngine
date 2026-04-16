#version 330 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec3 u_LightDir;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;

#define MAX_POINT_LIGHTS 16

struct PointLight {
    vec3  position;
    vec3  color;
    float intensity;
    float radius;
};

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int        u_NumPointLights;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // attenuation — light falls off based on radius
    float attenuation = clamp(1.0 - (distance / light.radius), 0.0, 1.0);
    attenuation *= attenuation; // square it for more natural falloff

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * light.color * light.intensity * 0.3;

    return (diffuse + specular) * attenuation;
}

void main() {
    vec3 texColor = texture(u_Texture, v_TexCoord).rgb;
    vec3 norm     = normalize(v_Normal);
    vec3 viewDir  = normalize(u_ViewPos - v_FragPos);

    // ambient
    vec3 ambient = 0.05 * u_LightColor;

    // directional light
    vec3 lightDir = normalize(u_LightDir);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * u_LightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec      = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular   = 0.3 * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * texColor;

    // point lights
    for (int i = 0; i < u_NumPointLights; i++) {
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir) * texColor;
    }

    FragColor = vec4(result, 1.0);
}