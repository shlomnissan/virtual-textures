// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <memory>
#include <set>
#include <vector>

#include <glm/vec2.hpp>

#include "core/orthographic_camera.h"
#include "loaders/image_loader.h"
#include "page.h"
#include "types.h"

class PageManager {
public:
    PageManager(
        const glm::vec2& texture_size,
        const glm::vec2& page_size,
        const int lods
    );

    auto IngestPages(const std::vector<unsigned>& page_data) -> void;

    auto GetVisiblePages() -> std::vector<Page*>;

    auto Debug(const OrthographicCamera& camera) const -> void;

private:
    std::set<PageId> visible_pages_cache_;
    std::vector<int> pages_x_per_lod_;
    std::vector<int> pages_y_per_lod_;
    std::vector<std::vector<Page>> pages_;

    std::shared_ptr<ImageLoader> loader_;

    glm::vec2 texture_size_;
    glm::vec2 page_size_;

    unsigned lods_ {0};
    unsigned curr_lod_ {0};

    auto GeneratePages() -> void;

    auto GetPageId(unsigned packed_data) const -> PageId;

    auto GetPageIndex(const PageId& id) const -> int;

    auto RequestPage(const PageId& id) -> void;

    auto GetLowResPages() -> std::vector<Page>&;
};