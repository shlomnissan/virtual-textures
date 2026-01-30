#version 410 core
#pragma inject_attributes

in vec3 a_Position;
in vec2 a_TexCoord;

out vec2 v_TexCoord;

layout(std140) uniform ub_Camera {
    mat4 u_Projection;
    mat4 u_View;
};

uniform mat4 u_Model;

mat4 u_ModelView = u_View * u_Model;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = u_Projection * u_ModelView * vec4(a_Position, 1.0);
}