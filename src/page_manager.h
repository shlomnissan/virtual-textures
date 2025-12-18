// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <format>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <glm/vec2.hpp>

#include "core/image_loader.h"
#include "core/texture2d.h"

#include "page_tables.h"
#include "page_cache.h"
#include "types.h"

constexpr auto pages = glm::ivec2(8, 8);
constexpr auto page_size = glm::vec2(512.0f, 512.0f);
constexpr auto padding = glm::vec2(4.0f, 4.0f);
constexpr auto physical_page_size = page_size + padding;
constexpr auto atlas_size = physical_page_size * glm::vec2(pages);
constexpr auto min_pinned_lod_idx = 3u;

struct PendingUpload {
    PageRequest request;
    PageSlot page_slot;
    std::shared_ptr<Image> image;
};

struct PendingFailure {
    PageRequest request;
    PageSlot page_slot;
};

struct PageManager {
    PageCache page_cache {pages.x, pages.y, min_pinned_lod_idx};

    PageTables page_table;

    Texture2D atlas {};

    std::vector<PendingUpload> upload_queue {};
    std::vector<PendingFailure> failure_queue {};

    std::mutex upload_mutex {};

    std::shared_ptr<ImageLoader> loader {ImageLoader::Create()};

    std::set<PageRequest> processing {};

    size_t alloc_idx = 0;

    PageManager(const glm::vec2& virtual_size, unsigned int lods) : page_table(virtual_size / page_size) {
        atlas.InitTexture({
            .width = static_cast<int>(atlas_size.x),
            .height = static_cast<int>(atlas_size.y),
            .levels = 0,
            .internal_format = GL_RGBA,
            .format = GL_RGBA,
            .type = GL_UNSIGNED_BYTE,
            .min_filter = GL_LINEAR,
            .data = nullptr
        });

        // preload pinned pages
        for (auto lod = min_pinned_lod_idx; lod < lods; ++lod) {
            auto rows = std::max(static_cast<int>(virtual_size.y / page_size.y) >> lod, 1);
            auto cols = std::max(static_cast<int>(virtual_size.x / page_size.x) >> lod, 1);
            for (auto row = 0; row < rows; row++) {
                for (auto col = 0; col < cols; col++) {
                    RequestPage(PageRequest {.lod = lod, .x = col, .y = row});
                }
            }
        }
    }

    auto IngestFeedback(const std::vector<uint32_t>& feedback) {
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
            page_cache.Touch(request);

            if (
                !page_table.IsResident(request.lod, request.x, request.y) &&
                processing.find(request) == processing.end()
            ) {
                RequestPage(request);
            }
        }
    }

    auto FlushUploadQueue() -> void {
        std::vector<PendingUpload> uploads;
        std::vector<PendingFailure> failures;

        {
            auto lock = std::lock_guard(upload_mutex);
            uploads.swap(upload_queue);
            failures.swap(failure_queue);
        }

        for (const auto& f : failures) {
            page_cache.Cancel(f.page_slot);
            processing.erase(f.request);
        }

        for (const auto& u : uploads) {
            atlas.Update(
                physical_page_size.x * u.page_slot.x,
                physical_page_size.y * u.page_slot.y,
                physical_page_size.x,
                physical_page_size.y,
                u.image->Data()
            );

            auto entry = uint32_t {
                0x1 | ((u.page_slot.x & 0xFFu) << 1) | ((u.page_slot.y & 0xFFu) << 9)
            };

            page_table.Write(u.request, entry);
            page_cache.Commit(u.request, u.page_slot);
            processing.erase(u.request);
        }
    }

    auto RequestPage(const PageRequest& request) -> void {
        auto alloc_result = page_cache.Acquire(request);
        if (!alloc_result.slot) {
            std::println("No evictable slot available at the moment");
            processing.erase(request);
            return;
        }

        if (alloc_result.evicted) {
            page_table.Write(alloc_result.evicted.value(), 0u);
        }

        auto slot = alloc_result.slot.value();
        auto path = std::format("assets/pages/{}_{}_{}.png", request.lod, request.x, request.y);
        processing.emplace(request);

        loader->LoadAsync(path, [this, request, slot](auto loader_result) {
            auto lock = std::lock_guard(upload_mutex);
            if (loader_result) {
                upload_queue.emplace_back(
                    request,
                    slot,
                    std::move(loader_result.value())
                );
            } else {
                std::println("{}", loader_result.error());
                failure_queue.emplace_back(request, slot);
            }

        });
    }
};