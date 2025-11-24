// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <functional>
#include <memory>
#include <string>

using ImageData = std::unique_ptr<unsigned char[], std::function<void(void*)>>;

class Image {
public:
    struct Parameters {
        std::string filename {};
        int width {0};
        int height {0};
        int depth {0};
    };

    std::string filename {};

    unsigned int width {0};
    unsigned int height {0};
    unsigned int depth {0};

    Image(const Parameters& params, ImageData data) :
        filename(params.filename),
        width(params.width),
        height(params.height),
        depth(params.depth),
        data_(std::move(data)) {}

    Image(Image&& other) noexcept :
        filename(std::move(other.filename)),
        width(other.width),
        height(other.height),
        depth(other.depth),
        data_(std::move(other.data_))
    {
        Reset(other);
    }

    auto operator=(Image&& other) noexcept -> Image& {
        if (this != &other) {
            data_ = std::move(other.data_);
            filename = std::move(other.filename);
            width = other.width;
            height = other.height;
            depth = other.depth;
            Reset(other);
        }
        return *this;
    }

    // deleted copy constructors and assignment operators
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    [[nodiscard]] auto Data() const { return data_.get(); }

    ~Image() = default;

private:
    ImageData data_ {nullptr, [](void*){}};

    auto Reset(Image& instance) const -> void {
        instance.data_ = nullptr;
        instance.filename.clear();
        instance.width = 0;
        instance.height = 0;
        instance.depth = 0;
    }
};