#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) out vec4 FragColor;

uniform sampler2D u_Texture0;

in vec2 v_TexCoord;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}