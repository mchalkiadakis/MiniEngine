#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_TexCoords;

void main() {
    v_TexCoords = aPos;
    vec4 pos = u_Projection * u_View * vec4(aPos, 1.0);
    // trick: set z to w so depth is always 1.0 (maximum)
    // this ensures skybox is always behind everything
    gl_Position = pos.xyww;
}