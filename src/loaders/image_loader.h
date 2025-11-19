// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include "core/image.h"
#include "loaders/loader.h"

#include <filesystem>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

using ImageCallback = std::function<void(std::shared_ptr<Image>)>;

class ImageLoader : public Loader<Image> {
public:
    [[nodiscard]] static auto Create() -> std::shared_ptr<ImageLoader> {
        return std::shared_ptr<ImageLoader>(new ImageLoader());
    }

    ~ImageLoader() override = default;

private:
    ImageLoader() = default;

    [[nodiscard]] auto ValidFileExtensions() const -> std::vector<std::string> override;

    [[nodiscard]] auto LoadImpl(const fs::path& path) const -> std::shared_ptr<void> override;
};