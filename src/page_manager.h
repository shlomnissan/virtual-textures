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
#include "page_allocator.h"

constexpr auto atlas_size = glm::vec2(4096.0f, 4096.0f);
constexpr auto page_size = glm::vec2(1024.0f, 1024.0f);
constexpr auto pages = glm::ivec2(atlas_size / page_size);

struct PageRequest {
    uint32_t lod;
    int x;
    int y;

    auto operator<=>(const PageRequest&) const = default;
};

struct PendingUpload {
    PageRequest request;
    PageAlloc page_alloc;
    std::shared_ptr<Image> image;
};

struct PageManager {
    PageAllocator page_allocator {pages};

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
            .internal_format = GL_RGBA,
            .format = GL_RGBA,
            .type = GL_UNSIGNED_BYTE,
            .min_filter = GL_LINEAR,
            .gen_mipmaps = true,
            .data = nullptr
        });
    }

    auto IngestFeedback(const std::vector<uint32_t>& feedback) {
        std::set<PageRequest> requests {};

        for (auto packed : feedback) {
            if (packed != 0xFFFFFFFFu) {
                requests.emplace(
                    packed & 0x1Fu,
                    static_cast<int>((packed >> 5) & 0x3Fu),
                    static_cast<int>((packed >> 11) & 0x3Fu)
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
        auto alloc = page_allocator.Alloc();
        if (!alloc) {
            std::println(std::cerr, "{}", alloc.error());
            return;
        }

        auto path = std::format("assets/pages/{}_{}_{}.png", request.lod, request.x, request.y);

        loader->LoadAsync(path, [this, request, alloc](auto result) {
            if (result) {
                auto lock = std::lock_guard(upload_mutex);
                upload_queue.emplace_back(
                    request,
                    alloc.value(),
                    std::move(result.value())
                );
            } else {
                std::println("{}", result.error());
            }

        });
    }
};