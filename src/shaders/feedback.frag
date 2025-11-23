#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) out uvec4 o_Feedback;

in vec2 v_TexCoord;

uniform vec2 u_TextureSize;
uniform vec2 u_TileSize;
uniform float u_BufferScreenRatio;
uniform int u_MaxMipLevel;

uint PackTileData(uint mip, uint tileX, uint tileY) {
    mip = mip & 0x1Fu;
    tileX = tileX & 0x3Fu;
    tileY = tileY & 0x3Fu;
    return (mip) | (tileX << 5) | (tileY << 11);
}

void main() {
    vec2 texel_dx = dFdx(v_TexCoord) * u_TextureSize;
    vec2 texel_dy = dFdy(v_TexCoord) * u_TextureSize;

    float rho = max(length(texel_dx), length(texel_dy));
    float texel_footprint = max(rho * u_BufferScreenRatio, 1e-8); // avoid log2(0)
    float mip_f = clamp(log2(texel_footprint), 0.0, float(u_MaxMipLevel));
    uint  mip_level = uint(mip_f);

    vec2 tiles = u_TextureSize / u_TileSize;
    vec2 tile_mip = max(tiles / exp2(float(mip_level)), vec2(1.0));

    float tile_x = clamp(v_TexCoord.x * tile_mip.x, 0.0, tile_mip.x - 1.0);
    float tile_y = clamp(v_TexCoord.y * tile_mip.y, 0.0, tile_mip.y - 1.0);

    uint data = PackTileData(mip_level, uint(tile_x), uint(tile_y));

    o_Feedback = uvec4(data, 0, 0, 0);
}