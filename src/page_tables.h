// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <cassert>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "core/texture2d.h"

#include "types.h"

struct PageTables {
    std::vector<std::vector<uint32_t>> tables {};

    glm::ivec2 texture_size;
    glm::ivec2 page_size;

    Texture2D texture;

    int lods {0};
    int pages_x {0};
    int pages_y {0};

    PageTables(const glm::ivec2& texture_size, const glm::ivec2& page_size) :
        texture_size(texture_size),
        page_size(page_size)
    {
        assert(texture_size.x % page_size.x == 0);
        assert(texture_size.y % page_size.y == 0);

        pages_x = texture_size.x / page_size.x;
        pages_y = texture_size.y / page_size.y;

        auto x = pages_x;
        auto y = pages_y;
        while (true) {
            tables.emplace_back(x * y, 0u);

            if (x == 1 && y == 1) break;

            x = std::max(x >> 1, 1);
            y = std::max(y >> 1, 1);
        }

        lods = static_cast<int>(tables.size());

        texture.InitTexture({
            .width = pages_x,
            .height = pages_y,
            .levels = lods,
            .internal_format = GL_R32UI,
            .format = GL_RED_INTEGER,
            .type = GL_UNSIGNED_INT,
            .min_filter = GL_NEAREST,
            .data = nullptr
        });
    }

    auto IsResident(int lod, int page_x, int page_y) const {
        const auto row_width = pages_x >> lod;
        const auto idx = static_cast<size_t>(page_y) * row_width + page_x;
        return static_cast<bool>(tables[lod][idx] & 1);
    }

    auto Write(const PageRequest& request, uint32_t entry) {
        const auto row_width = pages_x >> request.lod;
        const auto idx = static_cast<size_t>(request.y) * row_width + request.x;
        tables[request.lod][idx] = entry;
    }

    auto Update() {
        for (auto i = 0; i < lods; ++i) {
            texture.Update(
                /* offset x = */ 0,
                /* offset x = */ 0,
                /* width = */ pages_x >> i,
                /* height = */ pages_y >> i,
                /* data = */ tables[i].data(),
                /* mip level = */ i
            );
        }
    }
};