// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <memory>
#include <set>
#include <vector>

#include <glm/vec2.hpp>

#include "core/orthographic_camera.h"
#include "loaders/image_loader.h"
#include "tile.h"
#include "types.h"

class TileManager {
public:
    TileManager(
        const glm::vec2& texture_size,
        const glm::vec2& tile_size,
        const int lods
    );

    auto IngestTiles(const std::vector<unsigned>& tile_data) -> void;

    auto GetVisibleTile() -> std::vector<Tile*>;

    auto Debug(const OrthographicCamera& camera) const -> void;

private:
    std::set<TileId> visible_tiles_cache_;
    std::vector<int> tiles_x_per_lod_;
    std::vector<int> tiles_y_per_lod_;
    std::vector<std::vector<Tile>> tiles_;

    std::shared_ptr<ImageLoader> loader_;

    glm::vec2 texture_size_;
    glm::vec2 tile_size_;

    unsigned lods_ {0};
    unsigned curr_lod_ {0};

    auto GenerateTiles() -> void;

    auto GetTileId(unsigned packed_data) const -> TileId;

    auto GetTileIndex(const TileId& id) const -> int;

    auto RequestTile(const TileId& id) -> void;

    auto GetLowResTiles() -> std::vector<Tile>&;
};