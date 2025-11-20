#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

void main() {
    gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
}