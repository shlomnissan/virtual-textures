// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include <array>
#include <vector>

#include <glm/vec2.hpp>

#include "core/framebuffer.h"

class FeedbackBuffer {
public:
    FeedbackBuffer(const glm::ivec2& size);

    auto Bind() const -> void;

    auto Unbind() const -> void;

    [[nodiscard]] auto Data() -> const std::vector<GLuint>&;

private:
    Framebuffer framebuffer_;

    Texture2D texture_;

    std::vector<GLuint> buffer_;
};