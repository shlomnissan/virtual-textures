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

    auto Bind(int unit) -> void;

    auto Read(void* dst) const -> void;

    auto Update(int offset_x, int offset_y, int width, int height, void* data) const -> void;

    [[nodiscard]] auto Id() const { return texture_id_; }

    [[nodiscard]] auto Width() const { return width_; }

    [[nodiscard]] auto Height() const { return height_; }

    ~Texture2D();

private:
    std::shared_ptr<Image> image_cache_ {nullptr};

    unsigned int texture_id_;
    unsigned int format_;
    unsigned int type_;

    int width_ {0};
    int height_ {0};


    bool is_loaded_ {false};
};