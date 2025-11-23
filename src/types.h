// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <glm/vec2.hpp>

struct Dimensions {
    float width {0};
    float height {0};

    auto AspectRatio() const { return width / height; }
};

struct TileId {
    unsigned lod;
    int x;
    int y;

    auto operator<=>(const TileId&) const = default;
};

template<>
struct std::formatter<TileId> : std::formatter<std::string> {
    auto format(const TileId& id, auto& ctx) const {
        return std::formatter<std::string>::format(
            std::format("{}_{}_{}", id.lod, id.x, id.y),
            ctx
        );
    }
};