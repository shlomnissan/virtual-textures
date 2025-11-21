#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) out uvec4 o_Feedback;

in vec2 v_TexCoord;

uniform float u_TextureSize;
uniform float u_PageSize;
uniform float u_BufferScreenRatio;
uniform int u_MaxMipLevel;

uint PackPageData(uint mip, uint pageX, uint pageY) {
    mip = mip & 0x1Fu;
    pageX = pageX & 0x3Fu;
    pageY = pageY & 0x3Fu;
    return (mip) | (pageX << 5) | (pageY << 11);
}

void main() {
    vec2 dx = dFdx(v_TexCoord);
    vec2 dy = dFdy(v_TexCoord);

    float uv_deriv = max(length(dx), length(dy));
    float texel_footprint = uv_deriv * u_TextureSize * u_BufferScreenRatio;
    texel_footprint = max(texel_footprint, 1e-8); // avoid log2(0)

    float mip = clamp(log2(texel_footprint), 0.0, float(u_MaxMipLevel));
    uint mip_level = uint(mip);

    float tiles_max = u_TextureSize / u_PageSize;
    uint tiles_mip  = uint(tiles_max / pow(2.0, float(mip_level)));
    tiles_mip = max(tiles_mip, 1u); // safety clamp

    float page_x = clamp(v_TexCoord.x * float(tiles_mip), 0.0, float(tiles_mip) - 1.0);
    float page_y = clamp(v_TexCoord.y * float(tiles_mip), 0.0, float(tiles_mip) - 1.0);

    uint data = PackPageData(mip_level, uint(page_x), uint(page_y));
    o_Feedback = uvec4(data, 0, 0, 0);
}