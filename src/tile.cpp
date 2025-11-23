// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "tile.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

auto Tile::Transform() const -> glm::mat4 {
    return glm::translate(glm::mat4(1.0f), glm::vec3 {
        position.x + size.x / 2.0f,
        position.y + size.y / 2.0f,
        0.0f
    }) * glm::scale(glm::mat4(1.0f), glm::vec3 {scale, scale, 1.0f});
}