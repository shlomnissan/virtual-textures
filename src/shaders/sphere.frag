#version 410 core
#pragma inject_attributes

layout (location = 0) out vec4 v_FragColor;

in vec4 v_Position;

const vec3 top_color = vec3(0.02, 0.03, 0.05);
const vec3 bottom_color = vec3(0.005, 0.005, 0.01);

void main() {
    float y_pos = normalize(v_Position).y * 0.5 + 0.5;
    float f = pow(y_pos, 3.0);
    v_FragColor = vec4(mix(bottom_color, top_color, f), 1.0);
}