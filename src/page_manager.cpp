// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "page_manager.h"

#include <format>
#include <print>

#include <imgui.h>

PageManager::PageManager(
    const Dimensions& image_dims,
    const Dimensions& window_dims,
    const float page_size,
    const int lods
) :
    loader_(ImageLoader::Create()),
    image_dims_(image_dims),
    window_dims_(window_dims),
    page_size_(page_size),
    max_lod_(lods - 1)
{
    tiles_x_per_lod_.resize(lods);
    tiles_y_per_lod_.resize(lods);
    pages_.resize(lods);
    GeneratePages();
}

auto PageManager::Update(const OrthographicCamera& camera) -> void {
    const auto this_lod = ComputeLod(camera);

    if (first_frame_) {
        prev_lod_ = this_lod;
        curr_lod_ = this_lod;
        first_frame_ = false;
    }

    if (this_lod != curr_lod_) {
        prev_lod_ = curr_lod_;
        curr_lod_ = this_lod;
    }

    const auto visible_bounds = ComputeVisibleBounds(camera);
    for (auto lod = 0; lod <= max_lod_; ++lod) {
        for (auto& page : pages_[lod]) {
            page.visible = IsPageVisible(page, visible_bounds);
            if (page.visible && lod == curr_lod_ && page.state == PageState::Unloaded) {
                RequestPage(page.id);
            }
        }
    }
}

auto PageManager::GetVisiblePages() -> std::vector<Page*> {
    std::vector<Page*> visible_pages;

    // always include low-res tiles
    for (auto& page : pages_[max_lod_]) {
        if (page.visible && page.state == PageState::Loaded) {
            visible_pages.push_back(&page);
        }
    }

    if (curr_lod_ == max_lod_) return visible_pages;

    for (auto& page : pages_[curr_lod_]) {
        if (page.state == PageState::Loaded) {
            visible_pages.push_back(&page);
        }
    }

    return visible_pages;
}

auto PageManager::Debug() const -> void {
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Page Manager");

    ImGui::Text(
        "Image dimensions: %dx%d",
        static_cast<int>(image_dims_.width),
        static_cast<int>(image_dims_.height)
    );

    ImGui::Text("Current LOD: %d", curr_lod_);
    ImGui::End();
}

auto PageManager::GeneratePages() -> void {
    for (auto lod = 0u; lod <= max_lod_; ++lod) {
        auto lod_w = image_dims_.width / static_cast<float>(1 << lod);
        auto lod_h = image_dims_.height / static_cast<float>(1 << lod);
        auto lod_scale = static_cast<float>(pow(2, lod));
        auto tiles_x = static_cast<int>(std::ceil(lod_w / page_size_));
        auto tiles_y = static_cast<int>(std::ceil(lod_h / page_size_));

        tiles_x_per_lod_[lod] = tiles_x;
        tiles_y_per_lod_[lod] = tiles_y;

        pages_[lod].reserve(tiles_x * tiles_y);

        for (auto y = 0; y < tiles_y; ++y) {
            for (auto x = 0; x < tiles_x; ++x) {
                auto id = PageId {lod, x, y};

                auto size = glm::vec2 {
                    page_size_ * lod_scale,
                    page_size_ * lod_scale,
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

auto PageManager::ComputeLod(const OrthographicCamera& camera) const -> int {
    auto scale_x = glm::length(glm::vec3{camera.transform[0]});
    auto virtual_width = camera.Width() * scale_x;
    auto world_units_per_pixel = virtual_width / window_dims_.width;
    auto lod = std::log2(world_units_per_pixel);
    return std::clamp(static_cast<int>(lod), 0, static_cast<int>(max_lod_));
}

auto PageManager::IsPageVisible(const Page& page, const Box2& visible_bounds) const -> bool {
    auto bounds = Box2 {.min = page.position, .max = page.position + page.size};
    return visible_bounds.Intersects(bounds);
}

auto PageManager::ComputeVisibleBounds(const OrthographicCamera& camera) const -> Box2 {
    auto inv_vp = glm::inverse(camera.projection * camera.View());
    auto top_left = inv_vp * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
    auto bottom_right = inv_vp * glm::vec4(1.0f, -1.0f, 0.0, 1.0f);
    return Box2::FromPoints(
        {top_left.x, top_left.y},
        {bottom_right.x, bottom_right.y}
    );
}

auto PageManager::GetPageIndex(const PageId& id) const -> int {
    return id.y * tiles_x_per_lod_[id.lod] + id.x;
}

auto PageManager::RequestPage(const PageId& id) -> void {
    const auto idx = GetPageIndex(id);
    const auto path = std::format("assets/tiles/{}.png", id);

    pages_[id.lod][idx].state = PageState::Loading;
     loader_->LoadAsync(path, [this, id, idx](auto result) {
        if (result) {
            pages_[id.lod][idx].texture.SetImage(result.value());
            pages_[id.lod][idx].state = PageState::Loaded;
        } else {
            pages_[id.lod][idx].state = PageState::Unloaded;
            std::println("Failed to load tile {}", id.lod);
        }
    });
}