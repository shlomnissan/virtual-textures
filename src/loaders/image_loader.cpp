// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#define STB_IMAGE_IMPLEMENTATION

#include "image_loader.h"

#include <iostream>

#include <stb_image.h>

auto ImageLoader::ValidFileExtensions() const -> std::vector<std::string> {
    return {".png", ".jpg", ".jpeg"};
}

auto ImageLoader::LoadImpl(const fs::path& path) const -> std::shared_ptr<void> {
    auto width = 0;
    auto height = 0;
    auto depth = 0;
    auto data = stbi_load(path.string().c_str(), &width, &height, &depth, 4);

    if (data == nullptr) {
        std::cerr << "Failed to load image '" << path.string() << "'\n";
        return nullptr;
    }

    return std::make_shared<Image>(Image {{
        .filename = path.filename().string(),
        .width = width,
        .height = height,
        .depth = depth
    }, ImageData(data, &stbi_image_free)});
}