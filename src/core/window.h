// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <functional>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/timer.h"

class Window {
public:
    Window(int width, int height, std::string_view title);

    auto Start(const std::function<void(const double delta)>& program) -> void;

    auto BufferWidth() const { return buffer_width_; }
    auto BufferHeight() const { return buffer_height_; }

    ~Window();

private:
    int buffer_width_ = 0;
    int buffer_height_ = 0;

    GLFWwindow* window_ {nullptr};
    Timer timer_ {};
};