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

    auto Bind() -> void;

    [[nodiscard]] auto IsLoaded() const -> bool {
        return is_loaded_;
    }

    ~Texture2D();

private:
    std::shared_ptr<Image> image_ {nullptr};

    auto InitTexture(std::shared_ptr<Image> image) -> void;

    unsigned int texture_id_;

    bool is_loaded_ {false};
};