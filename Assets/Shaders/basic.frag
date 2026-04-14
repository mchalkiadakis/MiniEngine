#version 330 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec3 u_LightDir;      // direction light is coming FROM, not pointing to
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;       // camera position for specular
// later i need to compute it on the CPU and pass it as a separate uniform
void main() {
    vec3 texColor = texture(u_Texture, v_TexCoord).rgb;
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightDir);

    // ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * u_LightColor;

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // specular
    float specularStrength = 0.3;
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); // Blinn-Phong halfway vector
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * texColor;
    FragColor = vec4(result, 1.0);
}