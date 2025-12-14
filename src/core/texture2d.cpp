// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "texture2d.h"

#include <glad/glad.h>

auto Texture2D::InitTexture(const Parameters& params) -> void {
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    params_ = params;

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        params.internal_format,
        params.width,
        params.height,
        0,
        params.format,
        params.type,
        params.data
    );

     for (int level = 1; level < params.levels; ++level) {
        glTexImage2D(
            GL_TEXTURE_2D,
            level,
            GL_R32UI,
            params.width >> level,
            params.height >> level,
            0,
            GL_RED_INTEGER,
            GL_UNSIGNED_INT,
            nullptr
        );
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    is_loaded_ = true;
}

auto Texture2D::Update(int offset_x, int offset_y, int width, int height, void* data, unsigned mip) const -> void {
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        mip,
        offset_x,
        offset_y,
        width,
        height,
        params_.format,
        params_.type,
        data
    );
}

auto Texture2D::Bind(int unit) -> void {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}

auto Texture2D::Read(void* dst) const -> void {
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glGetTexImage(GL_TEXTURE_2D, 0, params_.format, params_.type, dst);
}

Texture2D::~Texture2D() {
    if (is_loaded_) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}