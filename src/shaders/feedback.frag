#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) out uvec4 o_Feedback;

in vec2 v_TexCoord;

uniform vec2 u_VirtualSize;
uniform vec2 u_PageSize;
uniform vec2 u_MinMaxMipLevel;
uniform float u_BufferScreenRatio;

uint PackPageData(uint mip, uint pageX, uint pageY) {
    mip = mip & 0x1Fu;
    pageX = pageX & 0x3Fu;
    pageY = pageY & 0x3Fu;
    return (mip) | (pageX << 5) | (pageY << 11);
}

uint GetMipLevel() {
    vec2 texel_dx = dFdx(v_TexCoord) * u_VirtualSize;
    vec2 texel_dy = dFdy(v_TexCoord) * u_VirtualSize;
    float rho = max(length(texel_dx), length(texel_dy));
    float texel_footprint = max(rho * u_BufferScreenRatio, 1e-8);
    float mip_f = clamp(log2(texel_footprint), u_MinMaxMipLevel.x, u_MinMaxMipLevel.y);
    return uint(mip_f);
}

vec2 GetPagesPerMip(uint mip_level) {
    vec2 pages = u_VirtualSize / u_PageSize;
    return max(pages / exp2(float(mip_level)), vec2(1.0));
}

void main() {
    uint mip_level = GetMipLevel();
    vec2 pages_per_mip = GetPagesPerMip(mip_level);

    float page_x_f = clamp(v_TexCoord.x * pages_per_mip.x, 0.0, pages_per_mip.x - 1.0);
    float page_y_f = clamp(v_TexCoord.y * pages_per_mip.y, 0.0, pages_per_mip.y - 1.0);

    uint data = PackPageData(mip_level, uint(page_x_f), uint(page_y_f));

    o_Feedback = uvec4(data, 0, 0, 0);
}