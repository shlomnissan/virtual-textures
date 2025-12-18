// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "feedback_buffer.h"

namespace {

constexpr auto clear = std::array<GLuint, 4>{0xFFFFFFFFu, 0, 0, 0};

}

FeedbackBuffer::FeedbackBuffer(const glm::ivec2& size)
  : framebuffer_(size.x, size.y), buffer_(size.x * size.y)
{
    texture_.InitTexture({
        .width = framebuffer_.Width(),
        .height = framebuffer_.Height(),
        .levels = 0,
        .internal_format = GL_R32UI,
        .format = GL_RED_INTEGER,
        .type = GL_UNSIGNED_INT,
        .min_filter = GL_NEAREST,
        .data = nullptr
    });

    framebuffer_.AddColorAttachment(texture_.Id());
}

auto FeedbackBuffer::Bind() const -> void {
    framebuffer_.Bind();
    glViewport(0, 0, framebuffer_.Width(), framebuffer_.Height());
    glClearBufferuiv(GL_COLOR, 0, clear.data());
}

auto FeedbackBuffer::Unbind() const -> void {
    framebuffer_.Unbind();
}

auto FeedbackBuffer::Data() -> const std::vector<GLuint>& {
    texture_.Read(buffer_.data());
    return buffer_;
}