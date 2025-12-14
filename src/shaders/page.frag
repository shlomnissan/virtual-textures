#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) out vec4 FragColor;

uniform sampler2D u_TextureAtlas;
uniform usampler2D u_PageTable;

in vec2 v_TexCoord;

uniform vec2 u_VirtualSize;
uniform vec2 u_PageScale;
uniform vec2 u_PageGrid;
uniform vec2 u_MinMaxMipLevel;

const uint PAGE_MASK  = 0xFFu;

float ComputeMipLevel(in vec2 effective_size, in vec2 uv) {
    vec2 dx = dFdx(uv) * effective_size;
    vec2 dy = dFdy(uv) * effective_size;
    float texel_footprint = max(dot(dx, dx), dot(dy, dy));
    return 0.5 * log2(max(texel_footprint, 1e-8));
}

void main() {
    float mip_float = clamp(
        ComputeMipLevel(u_VirtualSize, v_TexCoord),
        u_MinMaxMipLevel.x,
        u_MinMaxMipLevel.y
    );

    int mip_level = int(mip_float);
    int max_level = int(u_MinMaxMipLevel.y);

    vec2 curr_page_grid = vec2(0.0);
    uint entry = 0u;
    bool is_resident = false;

    for (; mip_level <= max_level; ++mip_level) {
        float mip_scale = exp2(-float(mip_level));
        curr_page_grid = max(u_PageGrid * mip_scale, vec2(1.0));

        vec2 page_coords = floor(v_TexCoord * curr_page_grid);
        page_coords = clamp(page_coords, vec2(0.0), curr_page_grid - 1.0);

        // Flip Y to match coordinate systems:
        // The page table is indexed starting from top-left (row 0)
        page_coords.y = (curr_page_grid.y - 1) - page_coords.y;

        entry = texelFetch(u_PageTable, ivec2(page_coords), mip_level).r;
        if ((entry & 1u) != 0u) {
            is_resident = true;
            break; // Found a valid page
        }
    }

    if (!is_resident) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    ivec2 physical_page = ivec2(
        (entry >> 1) & PAGE_MASK,
        (entry >> 9) & PAGE_MASK
    );

    vec2 local_uv = fract(v_TexCoord * curr_page_grid);
    vec2 atlas_uv = (vec2(physical_page) + local_uv) * u_PageScale;

    vec2 dx = dFdx(v_TexCoord) * curr_page_grid * u_PageScale;
    vec2 dy = dFdy(v_TexCoord) * curr_page_grid * u_PageScale;

    FragColor = textureGrad(u_TextureAtlas, atlas_uv, dx, dy);
}
