#version 410 core
#pragma inject_attributes

layout (location = 0) out uvec4 v_FragColor;

in vec2 v_TexCoord;

uniform vec2 u_VirtualSize;
uniform vec2 u_PageGrid;
uniform vec2 u_MinMaxMipLevel;
uniform float u_BufferScreenRatio;

const uint VALID_BIT = 1u << 31;
const uint MIP_MASK  = 0x1Fu;
const uint PAGE_MASK = 0xFFu;

uint PackPageData(in uint mip, in uint page_x, in uint page_y) {
    return VALID_BIT |
          (mip & MIP_MASK) |
          ((page_x & PAGE_MASK) << 5) |
          ((page_y & PAGE_MASK) << 13);
}

float ComputeMipLevel(in vec2 effective_size, in vec2 uv) {
    vec2 dx = dFdx(uv) * effective_size;
    vec2 dy = dFdy(uv) * effective_size;
    float texel_footprint = max(dot(dx, dx), dot(dy, dy));
    return 0.5 * log2(max(texel_footprint, 1e-8));
}

void main() {
    vec2 effective_size = u_VirtualSize * u_BufferScreenRatio;

    uint mip_level = uint(clamp(
        ComputeMipLevel(effective_size, v_TexCoord),
        u_MinMaxMipLevel.x,
        u_MinMaxMipLevel.y
    ));

    float mip_scale = exp2(-float(mip_level));
    vec2 curr_page_grid = max(u_PageGrid * mip_scale, vec2(1.0));

    vec2 page_coords = floor(v_TexCoord * curr_page_grid);
    page_coords.y = (curr_page_grid.y - 1) - page_coords.y;
    page_coords = clamp(page_coords, vec2(0.0), curr_page_grid - 1.0);

    uint data = PackPageData(mip_level, uint(page_coords.x), uint(page_coords.y));

    v_FragColor = uvec4(data, 0, 0, 0);
}