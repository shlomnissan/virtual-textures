// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <vector>

#include <glad/glad.h>

#include "core/texture2d.h"

struct PageTable {
    Texture2D texture;

    std::vector<uint32_t> page_table;

    int width = 0;
    int height = 0;

    PageTable(int width, int height) : page_table(width * height), width(width), height(height) {
        texture.InitTexture({
            .width = width,
            .height = height,
            .internal_format = GL_R32UI,
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
            .min_filter = GL_NEAREST,
            .gen_mipmaps = false,
            .data = nullptr
        });
    }

    auto IsResident(int page_x, int page_y) const {
        const auto idx = static_cast<size_t>(page_y) * width + page_x;
        return static_cast<bool>(page_table[idx] & 1);
    }

    auto Write(int page_x, int page_y, uint32_t entry) {
        const auto idx = static_cast<size_t>(page_y) * width + page_x;
        page_table[idx] = entry;
    }

    auto Update() {
        texture.Update(0, 0, width, height, page_table.data());
    }
};