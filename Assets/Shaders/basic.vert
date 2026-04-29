#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat3 u_NormalMatrix;
uniform mat4 u_LightSpaceMatrix;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec4 v_FragPosLightSpace;
out mat3 v_TBN;

void main() {
    gl_Position         = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
    v_TexCoord          = aTexCoord;
    v_FragPos           = vec3(u_Model * vec4(aPos, 1.0));
    v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_FragPos, 1.0);

    // build TBN matrix in world space
    vec3 T = normalize(u_NormalMatrix * aTangent);
    vec3 N = normalize(u_NormalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N); // re-orthogonalize
    vec3 B = cross(N, T);

    v_TBN = mat3(T, B, N);
}