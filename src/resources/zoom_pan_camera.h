// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include "core/event_dispatcher.h"
#include "core/orthographic_camera.h"

#include <memory>

#include <glm/vec2.hpp>

class ZoomPanCamera {
public:
    static constexpr float kPanSpeed {6.0f};

    explicit ZoomPanCamera(OrthographicCamera* camera);

    auto Update() -> void;

    ~ZoomPanCamera();

private:
    OrthographicCamera* camera_;

    std::shared_ptr<EventListener> mouse_event_listener_;

    glm::vec2 mouse_position_ {0.0f};
    glm::vec2 prev_position_ {0.0f};

    float zoom_factor_ {1.0f};
    float curr_scroll_ {0.0f};

    bool is_first_pan_ {true};
    bool is_panning_ {false};

    bool pan_ {false};
    bool zoom_ {true};

    auto Pan() -> void;
    auto Zoom() -> void;
};