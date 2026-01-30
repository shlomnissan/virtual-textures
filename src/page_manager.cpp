/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "page_manager.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <print>
#include <set>

PageManager::PageManager(ImageLoader* image_loader) : image_loader_(image_loader) {
    static const auto atlas_size = kSlotSize * kAtlasSlots;

    tex_atlas_ = vglx::DynamicTexture2D::Create({
        .width = static_cast<int>(atlas_size.x),
        .height = static_cast<int>(atlas_size.y),
        .mips = 1,
        .format = vglx::Texture::Format::SRGBA8,
        .color_space = vglx::Texture::ColorSpace::sRGB
    });

    tex_atlas_->min_filter = vglx::Texture::MinFilter::Linear;

    for (auto i = kMinPinnedLod; i < kLods; ++i) {
        auto rows = std::max(static_cast<int>(kVirtualSize.y / kPageSize.y) >> i, 1);
        auto cols = std::max(static_cast<int>(kVirtualSize.x / kPageSize.x) >> i, 1);
        for (auto row = 0; row < rows; row++) {
            for (auto col = 0; col < cols; col++) {
                RequestPage({.lod = i, .x = col, .y = row});
            }
        }
    }
}

auto PageManager::IngestFeedback(const std::span<const uint32_t> feedback) -> void {
    std::set<PageRequest> requests {};

    for (auto packed : feedback) {
        if ((packed & (1u << 31)) == 0u) continue; // empty

        packed &= ~(1u << 31); // strip valid bit
        requests.emplace(
            packed & 0x1Fu,
            static_cast<int>((packed >> 5)  & 0xFFu),
            static_cast<int>((packed >> 13) & 0xFFu)
        );
    }

    for (auto request : requests) {
        page_cache_.Touch(request);
        auto is_resident = page_tables_.IsResident(request);
        auto is_handled = requests_.contains(request);
        if (!is_resident && !is_handled) {
            RequestPage(request);
        }
    }

    page_tables_.SyncTables();
}

auto PageManager::RequestPage(const PageRequest& request) -> void {
    auto alloc_result = page_cache_.Acquire(request);
    if (!alloc_result.slot) {
        std::println("No evictable slot available at the moment");
        requests_.erase(request);
        return;
    }

    if (alloc_result.evicted) {
        page_tables_.Write(alloc_result.evicted.value(), 0u);
    }

    auto path = fs::path {std::format("assets/pages/{}_{}_{}.png", request.lod, request.x, request.y)};
    requests_.emplace(request);

    processing_requests_.emplace_back(ProcessingRequest {
        .request = request,
        .slot = alloc_result.slot.value(),
        .handle = image_loader_->LoadAsync(path)
    });
}

auto PageManager::FlushProcessingRequests() -> void {
    static const auto slot_size_x = static_cast<int>(kSlotSize.x);
    static const auto slot_size_y = static_cast<int>(kSlotSize.y);

    std::erase_if(processing_requests_, [this](auto& req){
        if (auto res = req.handle.TryTake()) {
            tex_atlas_->UpdateSubregion(
                0,
                slot_size_x * req.slot.x,
                slot_size_y * req.slot.y,
                slot_size_x,
                slot_size_y,
                res.value()
            );

            auto entry = uint32_t {
                0x1 | ((req.slot.x & 0xFFu) << 1) | ((req.slot.y & 0xFFu) << 9)
            };

            page_tables_.Write(req.request, entry);
            page_cache_.Commit(req.request, req.slot);
            requests_.erase(req.request);

            return true;
        }

        if (auto err = req.handle.TryError()) {
            std::println(stderr, "{}", err.value());
            return true;
        }
        return false;
    });
}

auto PageManager::GetAtlasTexture() -> std::shared_ptr<vglx::DynamicTexture2D> {
    return tex_atlas_;
}

auto PageManager::GetPageTablesTexture() -> std::shared_ptr<vglx::DynamicTexture2D> {
    return page_tables_.tex_tables_;
}