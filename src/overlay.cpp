/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024  - Present, Shlomi Nissan
===========================================================================
*/

#include "overlay.hpp"

namespace {

constexpr auto kPreviewSize = 128.0f;
constexpr auto kBorderSize = 4.0f;
constexpr auto kMargin = 10.0f;

auto create_quad(float size, const vglx::UnlitMaterial::Parameters& params, int render_order) {
    auto material = vglx::UnlitMaterial::Create(params);
    material->depth_test = false;

    auto quad = vglx::Mesh::Create(
        vglx::PlaneGeometry::Create({
            .width = size,
            .height = size,
            .orientation = vglx::PlaneGeometry::Orientation::FaceZ
        }),
        material
    );

    quad->render_order = render_order;

    const auto center = kMargin + kPreviewSize * 0.5f;
    quad->transform.SetPosition({center, center, -1.0f});

    // The pixel-space camera is y-down which mirrors the quad vertically.
    // Negative y-scale restores the texture orientation and screen winding.
    quad->transform.SetScale({1.0f, -1.0f, 1.0f});

    return quad;
}

}

Overlay::Overlay(std::shared_ptr<vglx::DynamicTexture2D> tex_atlas) {
    Add(create_quad(kPreviewSize + kBorderSize * 2.0f, {.color = 0x000000u}, 0));
    Add(create_quad(kPreviewSize, {.texture_map = tex_atlas}, 1));
}
