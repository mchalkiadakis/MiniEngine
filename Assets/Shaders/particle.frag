#version 330 core
in  vec4 v_Color;
out vec4 FragColor;

void main() {
    // soft circle using gl_PointCoord
    vec2  uv   = gl_PointCoord - 0.5;
    float dist = length(uv) * 2.0;
    float alpha = clamp(1.0 - dist, 0.0, 1.0);
    alpha = pow(alpha, 2.0);
    FragColor = vec4(v_Color.rgb, v_Color.a * alpha);
}