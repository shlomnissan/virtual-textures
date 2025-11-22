// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "page_manager.h"

#include <format>
#include <print>
#include <set>

#include <imgui.h>

namespace {

constexpr auto clear_value = 0xFFFFFFFFu;

}

PageManager::PageManager(
    const float image_size,
    const float page_size,
    const int lods
) :
    loader_(ImageLoader::Create()),
    image_size_(image_size),
    page_size_(page_size),
    lods_(lods)
{
    tiles_x_per_lod_.resize(lods);
    tiles_y_per_lod_.resize(lods);

    pages_.resize(lods);

    GeneratePages();

    const auto& low_res_pages = pages_[lods_ - 1];
    for (const auto& page : low_res_pages) {
        RequestPage(page.id);
    }
}

auto PageManager::GetPageId(unsigned packed) const -> PageId {
    return PageId {
        .lod = packed & 0x1Fu,
        .x = static_cast<int>((packed >> 5) & 0x3Fu),
        .y = static_cast<int>((packed >> 11) & 0x3Fu)
    };
}

auto PageManager::IngestPages(const std::vector<unsigned>& data) -> void {
    auto visible_pages = std::set<PageId> {};
    for (auto packed : data) {
        if (packed == clear_value) continue;
        visible_pages.emplace(GetPageId(packed));
    }

    curr_lod_ = visible_pages.begin()->lod;

    for (const auto& page_id : visible_pages) {
        auto& page = pages_[page_id.lod][GetPageIndex(page_id)];
        page.visible = true;
        if (page.state != PageState::Loaded) {
            RequestPage(page_id);
        }
    }
}

auto PageManager::GetVisiblePages() -> std::vector<Page*> {
    auto visible_pages = std::vector<Page*> {};
    visible_pages.reserve(64);

    auto& low_res_pages = pages_[lods_ - 1];
    for (auto& page : low_res_pages) {
        visible_pages.push_back(&page);
    }

    for (auto& page : pages_[curr_lod_]) {
        visible_pages.push_back(&page);
    }

    return visible_pages;
}

auto PageManager::Debug(const OrthographicCamera& camera) const -> void {
    auto camera_scale = glm::length(glm::vec3 {camera.transform[0]});

    ImGui::SetNextWindowFocus();
    ImGui::Begin("Page Manager");
    ImGui::Text("Image size: %d", static_cast<int>(image_size_));
    ImGui::Text("Current LOD: %d", curr_lod_);
    ImGui::Text("Camera width: %.2f", camera.Width() * camera_scale);

    ImGui::End();
}

auto PageManager::GeneratePages() -> void {
    for (auto lod = 0u; lod < lods_; ++lod) {
        auto lod_w = image_size_ / static_cast<float>(1 << lod);
        auto lod_h = image_size_ / static_cast<float>(1 << lod);
        auto tiles_x = static_cast<int>(std::ceil(lod_w / page_size_));
        auto tiles_y = static_cast<int>(std::ceil(lod_h / page_size_));
        auto lod_scale = 1.0f / static_cast<float>(tiles_x);

        tiles_x_per_lod_[lod] = tiles_x;
        tiles_y_per_lod_[lod] = tiles_y;

        pages_[lod].reserve(tiles_x * tiles_y);

        for (auto y = 0; y < tiles_y; ++y) {
            for (auto x = 0; x < tiles_x; ++x) {
                auto id = PageId {lod, x, y};

                auto size = glm::vec2 {
                    page_size_ * lod_scale,
                    page_size_ * lod_scale
                };

                auto position = glm::vec2 {
                    static_cast<float>(x) * size.x,
                    static_cast<float>(y) * size.y
                };

                pages_[lod].emplace_back(id, position, size, lod_scale);
            }
        }
    }
}

auto PageManager::GetPageIndex(const PageId& id) const -> int {
    return id.y * tiles_x_per_lod_[id.lod] + id.x;
}

auto PageManager::RequestPage(const PageId& id) -> void {
    const auto idx = GetPageIndex(id);
    const auto path = std::format("assets/tiles/{}.png", id);

    pages_[id.lod][idx].state = PageState::Loading;
     loader_->Load(path, [this, id, idx](auto result) {
        if (result) {
            pages_[id.lod][idx].texture.SetImage(result.value());
            pages_[id.lod][idx].state = PageState::Loaded;
        } else {
            pages_[id.lod][idx].state = PageState::Unloaded;
            std::println("Failed to load tile {}", id.lod);
        }
    });
}