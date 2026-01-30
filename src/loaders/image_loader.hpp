/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>

#include <vglx/vglx.hpp>

namespace fs = std::filesystem;

using ImageLoadHandle = vglx::LoadHandle<std::vector<uint8_t>>;

class ImageLoader {
public:
    explicit ImageLoader(vglx::LoadScheduler* scheduler);

    auto LoadAsync(const fs::path& path) -> ImageLoadHandle;

private:
    vglx::LoadScheduler* scheduler_ {nullptr};
};
