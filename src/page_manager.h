// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <algorithm>
#include <format>
#include <memory>
#include <random>
#include <vector>

#include "core/texture2d.h"
#include "loaders/image_loader.h"

#include "page_table.h"

constexpr int image_h = 4096;
constexpr int image_w = 4096;
constexpr int page_h = 1024;
constexpr int page_w = 1024;
constexpr int pages_x = 4;
constexpr int pages_y = 4;

struct PageManager {
    PageTable page_table {pages_x, pages_y};
    Texture2D atlas {};

    std::shared_ptr<ImageLoader> loader {ImageLoader::Create()};

    PageManager() {
        auto alloc_arr = std::vector<int>(pages_x * pages_y);
        for (size_t i = 0u; i < alloc_arr.size(); ++i) {
            alloc_arr[i] = static_cast<int>(i);
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::ranges::shuffle(alloc_arr, gen);

        atlas.InitTexture(
            image_w, image_h,
            GL_RGBA, GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );

        auto i = 0;
        for (auto y = 0; y < pages_y; ++y) {
            for (auto x = 0; x < pages_x; ++x) {
                auto alloc_x = alloc_arr[i] % pages_x;
                auto alloc_y = alloc_arr[i] / pages_x;
                RequestPage(x, y, alloc_x, alloc_y);
                ++i;
            }
        }
    }

    auto RequestPage(int page_x, int page_y, int alloc_x, int alloc_y) -> void {
        auto path = std::format("assets/pages/{}_{}_{}.png", 1, page_x, page_y);
        loader->Load(path, [this, page_x, page_y, alloc_x, alloc_y](auto result) {
            if (!result) return;

            atlas.Update(
                page_w * alloc_x,
                page_h * alloc_y,
                page_w,
                page_h,
                result.value()->Data()
            );

            auto entry = uint32_t {(alloc_x & 0xFFu) | ((alloc_y & 0xFFu) << 8)};
            page_table.Write(page_x, page_y, entry);
        });
    }

};