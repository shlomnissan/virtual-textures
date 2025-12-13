// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <format>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include <glm/vec2.hpp>

#include "core/texture2d.h"
#include "loaders/image_loader.h"

#include "page_tables.h"
#include "page_cache.h"
#include "types.h"

constexpr auto atlas_size = glm::vec2(4096.0f, 4096.0f);
constexpr auto page_size = glm::vec2(1024.0f, 1024.0f);
constexpr auto pages = glm::ivec2(atlas_size / page_size);

struct PendingUpload {
    PageRequest request;
    PageSlot page_alloc;
    std::shared_ptr<Image> image;
};

struct PageManager {
    PageCache page_cache {pages.x, pages.y};

    PageTables page_table;

    Texture2D atlas {};

    std::vector<PendingUpload> upload_queue {};

    std::mutex upload_mutex {};

    std::shared_ptr<ImageLoader> loader {ImageLoader::Create()};

    std::set<PageRequest> processing {};

    size_t alloc_idx = 0;

    PageManager(const glm::ivec2& virtual_size) : page_table(virtual_size, page_size) {
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
            if (
                !page_table.IsResident(request.lod, request.x, request.y) &&
                processing.find(request) == processing.end()
            ) {
                processing.emplace(request);
                RequestPage(request);
            }
        }
    }

    auto FlushUploadQueue() -> void {
        auto lock = std::lock_guard(upload_mutex);
        while (!upload_queue.empty()) {
            auto e = upload_queue.back();
            upload_queue.pop_back();

            atlas.Update(
                page_size.x * e.page_alloc.x,
                page_size.y * e.page_alloc.y,
                page_size.x,
                page_size.y,
                e.image->Data()
            );

            auto entry = uint32_t {
                0x1 | ((e.page_alloc.x & 0xFFu) << 1) | ((e.page_alloc.y & 0xFFu) << 9)
            };

            page_table.Write(e.request.lod, e.request.x, e.request.y, entry);
            processing.erase(e.request);
        }
    }

    auto RequestPage(const PageRequest& request) -> void {
        auto alloc_result = page_cache.Alloc(request);
        if (!alloc_result.slot) {
            std::println(std::cerr, "Out of memory");
            return;
        }

        auto slot = alloc_result.slot.value();
        auto path = std::format("assets/pages/{}_{}_{}.png", request.lod, request.x, request.y);

        loader->LoadAsync(path, [this, request, slot](auto loader_result) {
            if (loader_result) {
                auto lock = std::lock_guard(upload_mutex);
                upload_queue.emplace_back(
                    request,
                    slot,
                    std::move(loader_result.value())
                );
            } else {
                std::println("{}", loader_result.error());
            }

        });
    }
};