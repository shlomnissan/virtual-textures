// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <glm/mat4x4.hpp>

class OrthographicCamera {
public:
    glm::mat4 transform {1.0f};
    glm::mat4 projection {1.0f};

    OrthographicCamera(
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far
    );

    [[nodiscard]] auto View() const -> glm::mat4 {
        return glm::inverse(transform);
    }

    [[nodiscard]] auto Width() const -> float {
        return right_ - left_;
    }

    [[nodiscard]] auto Height() const -> float {
        return bottom_ - top_;
    }

private:
    float left_ {0.0f};
    float right_ {0.0f};
    float bottom_ {0.0f};
    float top_ {0.0f};
};