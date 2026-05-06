#version 330 core

layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  aTangent;
layout (location = 4) in ivec4 aBoneIds;
layout (location = 5) in vec4  aBoneWeights;

uniform mat4  u_Model;
uniform mat4  u_View;
uniform mat4  u_Projection;
uniform mat3  u_NormalMatrix;
uniform mat4  u_LightSpaceMatrix;
uniform mat4  u_BoneMatrices[256];
uniform float u_SnapStrength;

out vec3 v_FragPos;
out vec2 v_TexCoord;
out vec4 v_FragPosLightSpace;
out mat3 v_TBN;

void main() {
    // skinning
    mat4 skinMatrix = mat4(0.0);
    for (int i = 0; i < 4; i++) {
        if (aBoneIds[i] < 0) continue;
        skinMatrix += u_BoneMatrices[aBoneIds[i]] * aBoneWeights[i];
    }
    if (skinMatrix == mat4(0.0))
        skinMatrix = mat4(1.0);

    vec4 skinnedPos    = skinMatrix * vec4(aPos, 1.0);
    vec3 skinnedNormal = mat3(skinMatrix) * aNormal;
    vec3 skinnedTangent = mat3(skinMatrix) * aTangent;

    vec4 worldPos  = u_Model * skinnedPos;
    v_FragPos      = worldPos.xyz;
    v_TexCoord     = aTexCoords;
    v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;

    // TBN matrix for normal mapping
    vec3 T = normalize(u_NormalMatrix * skinnedTangent);
    vec3 N = normalize(u_NormalMatrix * skinnedNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    v_TBN = mat3(T, B, N);

    gl_Position = u_Projection * u_View * worldPos;

    // PS1 vertex snapping
    if (u_SnapStrength > 0.0) {
        vec4 snapped   = gl_Position;
        snapped.xyz   /= snapped.w;
        snapped.xy     = floor(snapped.xy * u_SnapStrength) / u_SnapStrength;
        snapped.xyz   *= snapped.w;
        gl_Position    = snapped;
    }
}