// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "core/texture2d.h"

#include "types.h"

class PageTables {
public:
    PageTables(const glm::ivec2& pages);

    auto Write(const PageRequest& request, uint32_t entry) -> void;

    auto Update() -> void;

    [[nodiscard]] auto Texture() -> Texture2D& { return texture_; }

    [[nodiscard]] auto IsResident(int lod, int page_x, int page_y) const -> bool;

private:
    std::vector<std::vector<uint32_t>> tables_ {};

    glm::ivec2 pages_;

    Texture2D texture_;

    int lods_ {0};
};