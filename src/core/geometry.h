// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <vector>

#include "core/shaders.h"

class Geometry {
public:
    Geometry(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data = {}
    );

    auto Draw(const Shaders& shader) const -> void;

protected:
    Geometry() = default;

    auto SetVertexData(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data = {}
    ) -> void;

private:
    unsigned int vao_ {0};
    unsigned int vbo_ {0};
    unsigned int ebo_ {0};
    unsigned int indices_size_ {0};

    auto ConfigureVertices(const std::vector<float>& vertex_data) -> void;
    auto ConfigureIndices(const std::vector<unsigned int>& index_data) -> void;
};