#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat3 u_NormalMatrix;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
    v_TexCoords = aTexCoord;
    v_FragPos   = vec3(u_Model * vec4(aPos, 1.0));
    v_Normal    = u_NormalMatrix * aNormal;
}