// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "geometry.h"

#include <iostream>

#include <glad/glad.h>

#define BUFFER_OFFSET(offset) ((void*)(offset * sizeof(GLfloat)))
#define STRIDE(stride) (sizeof(GLfloat) * stride)

Geometry::Geometry(
    const std::vector<float>& vertex_data,
    const std::vector<unsigned int>& index_data
) {
    SetVertexData(vertex_data, index_data);
}

auto Geometry::SetVertexData(
    const std::vector<float>& vertex_data,
    const std::vector<unsigned int>& index_data
) -> void {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    ConfigureVertices(vertex_data);
    if (!index_data.empty()) {
        ConfigureIndices(index_data);
    }

    // clean-up
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

auto Geometry::Draw(const Shaders& shader) const -> void {
    if (vao_ == 0) {
        std::cerr << "Geometry not initialized. Cannot draw." << std::endl;
        return;
    }

    shader.Use();
    glBindVertexArray(vao_);
    if (indices_size_ > 0) {
        glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

auto Geometry::ConfigureVertices(const std::vector<float>& vertex_data) -> void {
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertex_data.size() * sizeof(float),
        vertex_data.data(),
        GL_STATIC_DRAW
    );

    // vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE(8), BUFFER_OFFSET(0));

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE(8), BUFFER_OFFSET(3));

    // texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE(8), BUFFER_OFFSET(6));
}

auto Geometry::ConfigureIndices(const std::vector<unsigned int>& index_data) -> void {
    indices_size_ = index_data.size();

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        index_data.size() * sizeof(unsigned int),
        index_data.data(),
        GL_STATIC_DRAW
    );
}