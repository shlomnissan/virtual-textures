// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "framebuffer.h"

Framebuffer::Framebuffer(int width, int height) : width_(width), height_(height) {
    glGenFramebuffers(1, &id_);
}

auto Framebuffer::AddColorAttachment(
    GLuint texture_id,
    GLenum attachment
) const -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        attachment,
        GL_TEXTURE_2D,
        texture_id,
        0
    );
}

auto Framebuffer::Bind() const -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

auto Framebuffer::Unbind() const -> void {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto Framebuffer::IsComplete() const -> bool {
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

Framebuffer::~Framebuffer() {
    if (id_) glDeleteFramebuffers(1, &id_);
}