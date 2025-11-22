// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <memory>
#include <vector>

#include "core/orthographic_camera.h"
#include "loaders/image_loader.h"
#include "page.h"
#include "types.h"

class PageManager {
public:
    PageManager(
        const float image_size,
        const float page_size,
        const int lods
    );

    auto IngestPages(const std::vector<unsigned>& page_data) -> void;

    auto GetVisiblePages() -> std::vector<Page*>;

    auto Debug(const OrthographicCamera& camera) const -> void;

private:
    std::vector<int> tiles_x_per_lod_;
    std::vector<int> tiles_y_per_lod_;
    std::vector<std::vector<Page>> pages_;

    std::shared_ptr<ImageLoader> loader_;

    float image_size_;
    float page_size_ {0};

    unsigned lods_ {0};
    unsigned curr_lod_ {0};

    auto GeneratePages() -> void;

    auto GetPageId(unsigned packed_data) const -> PageId;

    auto GetPageIndex(const PageId& id) const -> int;

    auto RequestPage(const PageId& id) -> void;
};