// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <format>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "core/texture2d.h"
#include "loaders/image_loader.h"
#include "types.h"

enum class PageState {
    Unloaded,
    Loading,
    Loaded,
    Error
};

struct Page {
    PageId id;

    glm::vec2 position;
    glm::vec2 size;
    float scale;

    bool visible {false};

    PageState state {PageState::Unloaded};

    Texture2D texture;

    Page(
        const PageId& id,
        const glm::vec2 position,
        const glm::vec2 size,
        const float scale
    ) : id(id),
        position(position),
        size(size),
        scale(scale) {}

    [[nodiscard]] auto Transform() const -> glm::mat4;
};