#version 410 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
}