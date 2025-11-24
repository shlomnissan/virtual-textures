// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "texture2d.h"

#include <glad/glad.h>

#include <print>

Texture2D::Texture2D(std::shared_ptr<Image> image) {
    InitTexture(
        image->width,
        image->height,
        GL_RGBA,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image->Data()
    );
}

auto Texture2D::SetImage(std::shared_ptr<Image> image) -> void {
    image_cache_ = std::move(image);
    is_loaded_ = false;
}

auto Texture2D::InitTexture(
    int width,
    int height,
    unsigned internal_format,
    unsigned format,
    unsigned type,
    const void* data
) -> void {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    width_ = width;
    height_ = height;
    format_ = format;
    type_ = type;

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        width,
        height,
        0,
        format,
        type,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    is_loaded_ = true;
}

auto Texture2D::Bind() -> void {
    if (!is_loaded_ && image_cache_ == nullptr) {
        std::println("Attempting to bind a texture that is not loaded");
        return;
    }

    if (!is_loaded_ && image_cache_ != nullptr) {
        glDeleteTextures(1, &texture_id_);
        InitTexture(
            image_cache_->width,
            image_cache_->height,
            GL_RGBA,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            image_cache_->Data()
        );
        image_cache_ = nullptr;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}

auto Texture2D::Read(void* dst) const -> void {
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glGetTexImage(GL_TEXTURE_2D, 0, format_, type_, dst);
}

Texture2D::~Texture2D() {
    if (is_loaded_) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}