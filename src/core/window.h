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

    ~Window();

private:
    GLFWwindow* window_ {nullptr};
    Timer timer_ {};
};