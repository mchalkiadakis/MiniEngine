#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  aTangent;
layout (location = 4) in ivec4 aBoneIds;
layout (location = 5) in vec4  aBoneWeights;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;
uniform mat4 u_BoneMatrices[256];

void main() {
    mat4 skinMatrix = mat4(0.0);
    for (int i = 0; i < 4; i++) {
        if (aBoneIds[i] < 0) continue;
        skinMatrix += u_BoneMatrices[aBoneIds[i]] * aBoneWeights[i];
    }
    if (skinMatrix == mat4(0.0))
        skinMatrix = mat4(1.0);

    vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);
    gl_Position = u_LightSpaceMatrix * u_Model * skinnedPos;
}