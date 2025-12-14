// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <memory>

#include "image.h"

class Texture2D {
public:
    struct Parameters {
        int width;
        int height;
        int levels;
        unsigned int internal_format;
        unsigned int format;
        unsigned int type;
        unsigned int min_filter;
        void* data;
    };

    Texture2D() = default;

    auto InitTexture(const Parameters& params) -> void;

    auto Bind(int unit) -> void;

    auto Read(void* dst) const -> void;

    auto Update(int offset_x, int offset_y, int width, int height, void* data, unsigned mip = 0) const -> void;

    [[nodiscard]] auto Id() const { return texture_id_; }

    [[nodiscard]] auto Width() const { return params_.width; }

    [[nodiscard]] auto Height() const { return params_.height; }

    ~Texture2D();

private:
    std::shared_ptr<Image> image_cache_ {nullptr};

    Parameters params_;

    unsigned int texture_id_;


    bool is_loaded_ {false};
};