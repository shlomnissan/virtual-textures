#version 410 core
#pragma inject_attributes

layout (location = 0) out vec4 v_FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_TextureAtlas;
uniform usampler2D u_PageTable;

const vec2 u_VirtualSize = vec2(8192.0, 8192.0);
const vec2 u_PageGrid = vec2(16.0, 16.0);
const vec2 u_MinMaxMipLevel = vec2(0.0, 4.0);
const vec2 u_AtlasSize = vec2(4128.0, 4128.0);
const vec2 u_PageSize = vec2(512.0, 512.0);
const vec2 u_PagePadding = vec2(4.0, 4.0);

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
        v_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    ivec2 physical_page = ivec2(
        (entry >> 1) & PAGE_MASK,
        (entry >> 9) & PAGE_MASK
    );

    vec2 local_uv = fract(v_TexCoord * curr_page_grid);

    vec2 page_origin = vec2(physical_page) * (u_PageSize + u_PagePadding);;
    vec2 half_padding = u_PagePadding * 0.5;
    vec2 sample_texel = page_origin + half_padding + local_uv * u_PageSize;
    vec2 atlas_uv = sample_texel / u_AtlasSize;

    vec2 dx = dFdx(v_TexCoord) * curr_page_grid * (u_PageSize / u_AtlasSize);
    vec2 dy = dFdy(v_TexCoord) * curr_page_grid * (u_PageSize / u_AtlasSize);

    v_FragColor = textureGrad(u_TextureAtlas, atlas_uv, dx, dy);
}