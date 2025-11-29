// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "core/framebuffer.h"

#include <array>
#include <vector>

constexpr std::array<GLuint, 4> clear {0xFFFFFFFFu, 0, 0, 0};

struct FeedbackBuffer {
    Framebuffer framebuffer;

    Texture2D texture;

    std::vector<GLuint> buffer;

    FeedbackBuffer(int width, int height) : framebuffer(width, height), buffer(width * height) {
        texture.InitTexture({
            .width = framebuffer.Width(),
            .height = framebuffer.Height(),
            .internal_format = GL_R32UI,
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
            .min_filter = GL_NEAREST,
            .gen_mipmaps = false,
            .data = nullptr
        });

        framebuffer.AddColorAttachment(texture.Id());
    }

    auto Bind() const -> void {
        framebuffer.Bind();
        glViewport(0, 0, framebuffer.Width(), framebuffer.Height());
        glClearBufferuiv(GL_COLOR, 0, clear.data());
    }

    auto Data() -> const std::vector<GLuint>& {
        texture.Read(buffer.data());
        return buffer;
    }

    auto Unbind() const -> void {
        framebuffer.Unbind();
    }
};