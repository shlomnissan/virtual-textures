// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <vector>

#include <glad/glad.h>

#include "core/texture2d.h"

struct PageTable {
    Texture2D texture;

    PageTable(int width, int height) {
        texture.InitTexture(
            width,
            height,
            GL_R32UI,
            GL_RED_INTEGER,
            GL_UNSIGNED_INT,
            nullptr
        );
    }

    auto Write(int page_x, int page_y, uint32_t entry) {
        texture.Bind();
        texture.Update(page_x, page_y, 1, 1, &entry);
    }
};