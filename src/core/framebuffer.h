// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <glad/glad.h>

class Framebuffer {
public:
    Framebuffer(int width, int height);

    auto AddColorAttachment(
        GLuint texture_id,
        GLenum attachment = GL_COLOR_ATTACHMENT0
    ) const -> void;

    auto IsComplete() const -> bool;

    auto Bind() const -> void;

    ~Framebuffer();

private:
    GLuint id_;

    int width_;
    int height_;
};