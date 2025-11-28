#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) out vec4 FragColor;

uniform sampler2D  u_TextureAtlas;
uniform usampler2D u_PageTable;

uniform ivec2 u_NumPages;
uniform vec2 u_PageScale;

in vec2 v_TexCoord;

void main() {
    ivec2 page = ivec2(v_TexCoord * vec2(u_NumPages.x, u_NumPages.y));
    page = clamp(page, ivec2(0), u_NumPages - 1);
    page.y = (u_NumPages.y - 1) - page.y;

    uint entry = texelFetch(u_PageTable, page, 0).r;
    uint resident = entry & 0x1u;

    if (resident == 0u) {
        FragColor = vec4(0.0);
        return;
    }

    ivec2 physical_page = ivec2((entry >> 1) & 0xFFu, (entry >> 9) & 0xFFu);

    vec2 localUV = fract(v_TexCoord * vec2(u_NumPages));
    vec2 atlasUV = physical_page * u_PageScale + localUV * u_PageScale;

    FragColor = texture(u_TextureAtlas, atlasUV);
}
