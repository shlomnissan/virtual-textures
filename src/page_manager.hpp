/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <memory>
#include <set>
#include <vector>

#include <vglx/vglx.hpp>

#include "globals.hpp"
#include "page_tables.hpp"
#include "page_cache.hpp"

#include "loaders/image_loader.hpp"

struct ProcessingRequest {
    PageRequest request;
    PageSlot slot;
    ImageLoadHandle handle;
};

class PageManager {
public:
    explicit PageManager(ImageLoader* image_loader);

    auto IngestFeedback(const std::span<const uint32_t> feedback) -> void;

    auto RequestPage(const PageRequest& request) -> void;

    auto FlushProcessingRequests() -> void;

    auto GetAtlasTexture() -> std::shared_ptr<vglx::DynamicTexture2D>;

    auto GetPageTablesTexture() -> std::shared_ptr<vglx::DynamicTexture2D>;

private:
    PageCache page_cache_ {};

    PageTables page_tables_ {};

    std::shared_ptr<vglx::DynamicTexture2D> tex_atlas_ {nullptr};

    std::vector<ProcessingRequest> processing_requests_ {};

    std::set<PageRequest> requests_ {};

    ImageLoader* image_loader_ {nullptr};
};