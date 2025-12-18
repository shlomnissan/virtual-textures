// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "page_tables.h"

PageTables::PageTables(const glm::ivec2& pages) : pages_(pages) {
    auto x = pages_.x;
    auto y = pages_.y;

    while (true) {
        tables_.emplace_back(x * y, 0u);

        if (x == 1 && y == 1) break;

        x = std::max(x >> 1, 1);
        y = std::max(y >> 1, 1);
    }

    lods_ = static_cast<int>(tables_.size());

    texture_.InitTexture({
        .width = pages_.x,
        .height = pages_.y,
        .levels = lods_,
        .internal_format = GL_R32UI,
        .format = GL_RED_INTEGER,
        .type = GL_UNSIGNED_INT,
        .min_filter = GL_NEAREST,
        .data = nullptr
    });
}

auto PageTables::Write(const PageRequest& request, uint32_t entry) -> void {
    const auto row_width = pages_.x >> request.lod;
    const auto idx = static_cast<size_t>(request.y) * row_width + request.x;
    tables_[request.lod][idx] = entry;
}

auto PageTables::Update() -> void {
    for (auto i = 0; i < lods_; ++i) {
        texture_.Update(
            /* offset x = */ 0,
            /* offset x = */ 0,
            /* width = */ pages_.x >> i,
            /* height = */ pages_.y >> i,
            /* data = */ tables_[i].data(),
            /* mip level = */ i
        );
    }
}

auto PageTables::IsResident(int lod, int page_x, int page_y) const -> bool {
    const auto row_width = pages_.x >> lod;
    const auto idx = static_cast<size_t>(page_y) * row_width + page_x;
    return static_cast<bool>(tables_[lod][idx] & 1);
}