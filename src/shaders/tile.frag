#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) out vec4 FragColor;

uniform sampler2D u_Texture0;

in vec2 v_TexCoord;

void main() {
    FragColor = texture(u_Texture0, v_TexCoord);
}