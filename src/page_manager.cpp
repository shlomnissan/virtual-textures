// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "page_manager.h"

#include <format>
#include <print>

#include <glad/glad.h>

PageManager::PageManager(const Parameters& params)
    : page_cache_(slots_in_cache, min_pinned_lod_idx),
    page_tables_(params.virtual_size / params.page_size),
    page_size_(params.page_size),
    slot_size_(params.page_size + params.page_padding),
    atlas_size_(slot_size_ * glm::vec2(slots_in_cache))
{
    atlas_.InitTexture({
        .width = static_cast<int>(atlas_size_.x),
        .height = static_cast<int>(atlas_size_.y),
        .levels = 0,
        .internal_format = GL_RGBA,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
        .min_filter = GL_LINEAR,
        .data = nullptr
    });

    // preload pinned pages
    for (auto lod = min_pinned_lod_idx; lod < LODs(); ++lod) {
        auto rows = std::max(static_cast<int>(params.virtual_size.y / page_size_.y) >> lod, 1);
        auto cols = std::max(static_cast<int>(params.virtual_size.x / page_size_.x) >> lod, 1);
        for (auto row = 0; row < rows; row++) {
            for (auto col = 0; col < cols; col++) {
                RequestPage(PageRequest {.lod = lod, .x = col, .y = row});
            }
        }
    }
}

auto PageManager::IngestFeedback(const std::vector<uint32_t>& feedback) -> void {
    std::set<PageRequest> requests {};

    for (auto packed : feedback) {
        if (packed != 0xFFFFFFFFu) {
            requests.emplace(
                packed & 0x1Fu,
                static_cast<int>((packed >> 5) & 0xFFu),
                static_cast<int>((packed >> 13) & 0xFFu)
            );
        }
    }

    for (auto request : requests) {
        page_cache_.Touch(request);
        if (
            !page_tables_.IsResident(request.lod, request.x, request.y) &&
            processing_.find(request) == processing_.end()
        ) {
            RequestPage(request);
        }
    }
}

auto PageManager::FlushUploadQueue() -> void {
    std::vector<PendingUpload> uploads;
    std::vector<PendingFailure> failures;

    {
        auto lock = std::lock_guard(upload_mutex_);
        uploads.swap(upload_queue_);
        failures.swap(failure_queue_);
    }

    for (const auto& f : failures) {
        page_cache_.Cancel(f.page_slot);
        processing_.erase(f.request);
    }

    for (const auto& u : uploads) {
        atlas_.Update(
            slot_size_.x * u.page_slot.x,
            slot_size_.y * u.page_slot.y,
            slot_size_.x,
            slot_size_.y,
            u.image->Data()
        );

        auto entry = uint32_t {
            0x1 | ((u.page_slot.x & 0xFFu) << 1) | ((u.page_slot.y & 0xFFu) << 9)
        };

        page_tables_.Write(u.request, entry);
        page_cache_.Commit(u.request, u.page_slot);
        processing_.erase(u.request);
    }
}

auto PageManager::RequestPage(const PageRequest& request) -> void {
    auto alloc_result = page_cache_.Acquire(request);
    if (!alloc_result.slot) {
        std::println("No evictable slot available at the moment");
        processing_.erase(request);
        return;
    }

    if (alloc_result.evicted) {
        page_tables_.Write(alloc_result.evicted.value(), 0u);
    }

    auto slot = alloc_result.slot.value();
    auto path = std::format("assets/pages/{}_{}_{}.png", request.lod, request.x, request.y);
    processing_.emplace(request);

    loader_->LoadAsync(path, [this, request, slot](auto loader_result) {
        auto lock = std::lock_guard(upload_mutex_);
        if (loader_result) {
            upload_queue_.emplace_back(
                request,
                slot,
                std::move(loader_result.value())
            );
        } else {
            std::println("{}", loader_result.error());
            failure_queue_.emplace_back(request, slot);
        }

    });
}

auto PageManager::BindTexture(Texture type) -> void {
    if (type == Texture::Atlas) atlas_.Bind(0);
    if (type == Texture::PageTables) page_tables_.Texture().Bind(1);
}