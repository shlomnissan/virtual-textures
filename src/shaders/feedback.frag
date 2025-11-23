#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) out uvec4 o_Feedback;

in vec2 v_TexCoord;

uniform vec2 u_TextureSize;
uniform vec2 u_PageSize;
uniform float u_BufferScreenRatio;
uniform int u_MaxMipLevel;

uint PackPageData(uint mip, uint pageX, uint pageY) {
    mip = mip & 0x1Fu;
    pageX = pageX & 0x3Fu;
    pageY = pageY & 0x3Fu;
    return (mip) | (pageX << 5) | (pageY << 11);
}

void main() {
    vec2 texel_dx = dFdx(v_TexCoord) * u_TextureSize;
    vec2 texel_dy = dFdy(v_TexCoord) * u_TextureSize;

    float rho = max(length(texel_dx), length(texel_dy));
    float texel_footprint = max(rho * u_BufferScreenRatio, 1e-8); // avoid log2(0)
    float mip_f = clamp(log2(texel_footprint), 0.0, float(u_MaxMipLevel));
    uint  mip_level = uint(mip_f);

    vec2 pages = u_TextureSize / u_PageSize;
    vec2 page_mip_f = max(pages / exp2(float(mip_level)), vec2(1.0));

    float page_x_f = clamp(v_TexCoord.x * page_mip_f.x, 0.0, page_mip_f.x - 1.0);
    float page_y_f = clamp(v_TexCoord.y * page_mip_f.y, 0.0, page_mip_f.y - 1.0);

    uint data = PackPageData(mip_level, uint(page_x_f), uint(page_y_f));

    o_Feedback = uvec4(data, 0, 0, 0);
}