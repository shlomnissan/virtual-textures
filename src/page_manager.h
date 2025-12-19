// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once


#include <memory>
#include <mutex>

#include <set>
#include <vector>

#include <glm/vec2.hpp>

#include "core/image_loader.h"
#include "core/texture2d.h"

#include "page_cache.h"
#include "page_tables.h"
#include "types.h"

namespace {

constexpr auto slots_in_cache = glm::ivec2(8, 8);
constexpr auto min_pinned_lod_idx = 3u;

}


struct PendingUpload {
    PageRequest request;
    PageSlot page_slot;
    std::shared_ptr<Image> image;
};

struct PendingFailure {
    PageRequest request;
    PageSlot page_slot;
};

class PageManager {
public:
    struct Parameters {
        glm::vec2 virtual_size;
        glm::vec2 page_padding;
        glm::vec2 page_size;
    };

    enum class Texture {
        Atlas,
        PageTables,
    };

    explicit PageManager(const Parameters& params);

    auto IngestFeedback(const std::vector<uint32_t>& feedback) -> void;

    auto FlushUploadQueue() -> void;

    auto RequestPage(const PageRequest& request) -> void;

    auto BindTexture(Texture type) -> void;

    auto UpdatePageTables() -> void { page_tables_.Update(); }

    [[nodiscard]] auto AtlasSize() const -> glm::vec2 { return atlas_size_; }

    [[nodiscard]] auto LODs() const -> int { return page_tables_.LODs(); }

private:
    PageCache page_cache_;
    PageTables page_tables_;

    glm::vec2 page_size_ {0.0f};
    glm::vec2 slot_size_ {0.0f};
    glm::vec2 atlas_size_ {0.0f};

    Texture2D atlas_ {};

    std::mutex upload_mutex_ {};
    std::vector<PendingUpload> upload_queue_ {};
    std::vector<PendingFailure> failure_queue_ {};

    std::shared_ptr<ImageLoader> loader_ {ImageLoader::Create()};

    std::set<PageRequest> processing_ {};

    size_t alloc_idx_ = 0;
};