#version 330 core
layout (location = 0) in vec3  aPosition;
layout (location = 1) in vec4  aColor;
layout (location = 2) in float aSize;

uniform mat4  u_View;
uniform mat4  u_Projection;
uniform vec3  u_CamRight;
uniform vec3  u_CamUp;

out vec4 v_Color;

void main() {
    gl_Position  = u_Projection * u_View * vec4(aPosition, 1.0);
    gl_PointSize = aSize;
    v_Color      = aColor;
}