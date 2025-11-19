// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <glm/vec2.hpp>

struct Dimensions {
    float width {0};
    float height {0};

    auto AspectRatio() const { return width / height; }
};

struct Box2 {
    glm::vec2 min {0.0f};
    glm::vec2 max {0.0f};

    static auto FromPoints(const glm::vec2& a, const glm::vec2& b) {
        return Box2 {
            .min = glm::min(a, b),
            .max = glm::max(a, b),
        };
    }

    auto Intersects(const Box2& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y);
    }
};