// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "tile_manager.h"

#include <format>
#include <print>
#include <set>

#include <imgui.h>

namespace {

constexpr auto clear_value = 0xFFFFFFFFu;

}

TileManager::TileManager(
    const glm::vec2& texture_size,
    const glm::vec2& tile_size,
    const int lods
) :
    loader_(ImageLoader::Create()),
    texture_size_(texture_size),
    tile_size_(tile_size),
    lods_(lods)
{
    tiles_x_per_lod_.resize(lods);
    tiles_y_per_lod_.resize(lods);

    tiles_.resize(lods);

    GenerateTiles();

    for (auto& tile : GetLowResTiles()) {
        tile.visible = true;
        RequestTile(tile.id);
    }
}

auto TileManager::GetLowResTiles() -> std::vector<Tile>& {
    return tiles_[lods_ - 1];
}

auto TileManager::GetTileId(unsigned packed) const -> TileId {
    return TileId {
        .lod = packed & 0x1Fu,
        .x = static_cast<int>((packed >> 5) & 0x3Fu),
        .y = static_cast<int>((packed >> 11) & 0x3Fu)
    };
}

auto TileManager::IngestTiles(const std::vector<unsigned>& data) -> void {
    auto visible_tiles = std::set<TileId> {};
    for (auto packed : data) {
        if (packed == clear_value) continue;
        visible_tiles.emplace(GetTileId(packed));
    }

    // lod is consistent across tiles
    curr_lod_ = visible_tiles.begin()->lod;

    for (const auto& tile_id : visible_tiles_cache_) {
        tiles_[tile_id.lod][GetTileIndex(tile_id)].visible = false;
    }

    for (const auto& tile_id : visible_tiles) {
        auto& tile = tiles_[tile_id.lod][GetTileIndex(tile_id)];
        tile.visible = true;
        if (tile.state == TileState::Unloaded) {
            RequestTile(tile_id);
        }
    }
}

auto TileManager::GetVisibleTile() -> std::vector<Tile*> {
    auto visible_tiles = std::vector<Tile*> {};
    visible_tiles.reserve(64);

    // low-res tiles are always rendered
    for (auto& tile : GetLowResTiles()) {
        visible_tiles.push_back(&tile);
    }

    for (auto& tile : tiles_[curr_lod_]) {
        if (tile.visible && tile.state == TileState::Loaded) {
            visible_tiles.push_back(&tile);
        }
    }

    return visible_tiles;
}

auto TileManager::Debug(const OrthographicCamera& camera) const -> void {
    auto camera_scale = glm::length(glm::vec3 {camera.transform[0]});

    ImGui::SetNextWindowFocus();
    ImGui::Begin("Tile Manager");
    ImGui::Text("Image size: %d", static_cast<int>(texture_size_.x));
    ImGui::Text("Current LOD: %d", curr_lod_);
    ImGui::Text("Camera width: %.2f", camera.Width() * camera_scale);

    ImGui::End();
}

auto TileManager::GenerateTiles() -> void {
    for (auto lod = 0u; lod < lods_; ++lod) {
        auto lod_w = texture_size_.x / static_cast<float>(1 << lod);
        auto lod_h = texture_size_.y / static_cast<float>(1 << lod);
        auto tiles_x = static_cast<int>(std::ceil(lod_w / tile_size_.x));
        auto tiles_y = static_cast<int>(std::ceil(lod_h / tile_size_.y));
        auto lod_scale = 1.0f / static_cast<float>(tiles_x);

        tiles_x_per_lod_[lod] = tiles_x;
        tiles_y_per_lod_[lod] = tiles_y;

        tiles_[lod].reserve(tiles_x * tiles_y);

        for (auto y = 0; y < tiles_y; ++y) {
            for (auto x = 0; x < tiles_x; ++x) {
                auto id = TileId {lod, x, y};

                auto size = glm::vec2 {
                    tile_size_.x * lod_scale,
                    tile_size_.y * lod_scale
                };

                auto position = glm::vec2 {
                    static_cast<float>(x) * size.x,
                    static_cast<float>(y) * size.y
                };

                tiles_[lod].emplace_back(id, position, size, lod_scale);
            }
        }
    }
}

auto TileManager::GetTileIndex(const TileId& id) const -> int {
    return id.y * tiles_x_per_lod_[id.lod] + id.x;
}

auto TileManager::RequestTile(const TileId& id) -> void {
    const auto idx = GetTileIndex(id);
    const auto path = std::format("assets/tiles/{}.png", id);

    tiles_[id.lod][idx].state = TileState::Loading;
     loader_->LoadAsync(path, [this, id, idx](auto result) {
        if (result) {
            tiles_[id.lod][idx].texture.SetImage(result.value());
            tiles_[id.lod][idx].state = TileState::Loaded;
            std::println("Loaded tile {}", id);
        } else {
            tiles_[id.lod][idx].state = TileState::Unloaded;
            std::println("Failed to load tile {}", id);
        }
    });
}