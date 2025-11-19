// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "texture2d.h"

#include <glad/glad.h>

#include <iostream>

Texture2D::Texture2D(std::shared_ptr<Image> image) {
    InitTexture(image);
}

auto Texture2D::InitTexture(std::shared_ptr<Image> image) -> void {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image->width,
        image->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image->Data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    is_loaded_ = true;
}

auto Texture2D::SetImage(std::shared_ptr<Image> image) -> void {
    if (is_loaded_) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
    image_ = image;
    is_loaded_ = true;
}

auto Texture2D::Bind() -> void {
    if (texture_id_ == 0 && image_ != nullptr) {
        InitTexture(image_);
        image_ = nullptr;
    }

    if (texture_id_ == 0) {
        std::cerr << "Attempting to bind a texture that is not loaded\n";
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}

Texture2D::~Texture2D() {
    if (is_loaded_) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}