// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include "core/perspective_camera.h"
#include "core/event_dispatcher.h"
#include "core/events.h"

#include <memory>

#include <glm/vec3.hpp>

class OrbitControls {
public:
    float radius {0.0f};
    float phi {0.0f};
    float theta {0.0f};

    glm::vec3 target {0.0f};

    explicit OrbitControls(PerspectiveCamera* camera);

    ~OrbitControls();

    auto Update(float dt) -> void;

private:
    PerspectiveCamera* camera_;

    std::shared_ptr<EventListener> mouse_event_listener_;

    glm::vec2 curr_pos_ {0.0f, 0.0f};
    glm::vec2 prev_pos_ {0.0f, 0.0f};

    float scroll_offset_ {0.0f};

    MouseButton curr_button = {MouseButton::None};

    bool do_orbit_ {false};
    bool is_first_move_ {true};
};
