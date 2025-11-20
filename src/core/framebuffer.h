// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <glad/glad.h>

#include "core/texture2d.h"

class Framebuffer {
public:
    Framebuffer(int width, int height);

    auto AddColorAttachment(
        GLuint texture_id,
        GLenum attachment = GL_COLOR_ATTACHMENT0
    ) const -> void;

    auto IsComplete() const -> bool;

    auto Bind() const -> void;

    auto Unbind() const -> void;

    auto Width() const { return width_; }

    auto Height() const { return height_; }

    ~Framebuffer();

private:
    GLuint id_;

    int width_;
    int height_;
};