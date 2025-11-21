// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include "core/image.h"

#include <memory>

class Texture2D {
public:
    Texture2D() = default;

    explicit Texture2D(std::shared_ptr<Image> image);

    auto SetImage(std::shared_ptr<Image> image) -> void;

    auto InitTexture(
        int width,
        int height,
        unsigned internal_format,
        unsigned format,
        unsigned type,
        const void* data
    ) -> void;

    auto Bind() const -> void;

    auto Read(void* dst) const -> void;

    [[nodiscard]] auto Id() const { return texture_id_; }

    [[nodiscard]] auto Width() const { return width_; }

    [[nodiscard]] auto Height() const { return height_; }

    [[nodiscard]] auto IsLoaded() const -> bool {
        return is_loaded_;
    }

    ~Texture2D();

private:
    unsigned int texture_id_;

    int width_ {0};
    int height_ {0};

    unsigned format_;
    unsigned type_;

    bool is_loaded_ {false};
};