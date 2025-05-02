#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec2 aTexCoord;  // Texture coordinate (UV)

uniform mat4 u_Model;

out vec2 v_TexCoord; // Pass to fragment shader

void main() {
    gl_Position = u_Model * vec4(aPos, 1.0);
    v_TexCoord = aTexCoord;
}