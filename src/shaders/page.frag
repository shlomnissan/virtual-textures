#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) out vec4 FragColor;

uniform sampler2D u_TextureAtlas;
uniform usampler2D u_PageTable;

in vec2 v_TexCoord;

uniform vec2 u_VirtualSize;
uniform vec2 u_PageScale;
uniform vec2 u_PageSize;
uniform vec2 u_MinMaxMipLevel;

float MipLevel() {
    vec2 texel_dx = dFdx(v_TexCoord) * u_VirtualSize;
    vec2 texel_dy = dFdy(v_TexCoord) * u_VirtualSize;
    float rho = max(max(length(texel_dx), length(texel_dy)), 1e-8);
    return clamp(log2(rho), u_MinMaxMipLevel.x, u_MinMaxMipLevel.y);
}

ivec2 PagesPerMip(uint mip_level) {
    vec2 base_pages = u_VirtualSize / u_PageSize;
    vec2 pages = max(base_pages / exp2(float(mip_level)), vec2(1.0));
    return ivec2(pages);
}

void main() {
    int mip_level = int(MipLevel());
    ivec2 pages_per_mip = PagesPerMip(mip_level);

    ivec2 page = ivec2(v_TexCoord * vec2(pages_per_mip));
    page = clamp(page, ivec2(0), pages_per_mip - 1);
    page.y = (pages_per_mip.y - 1) - page.y;

    uint entry = texelFetch(u_PageTable, page, mip_level).r;
    uint resident = entry & 0x1u;

    if (resident == 0u) {
        FragColor = vec4(0.0);
        return;
    }

    ivec2 physical_page = ivec2(
        (entry >>  1) & 0xFFu,
        (entry >>  9) & 0xFFu
    );

    vec2 local_uv = fract(v_TexCoord * vec2(pages_per_mip));
    vec2 atlas_uv = (vec2(physical_page) + local_uv) * u_PageScale;

    FragColor = texture(u_TextureAtlas, atlas_uv);
}
