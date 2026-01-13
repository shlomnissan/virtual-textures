// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "orbit_controls.h"

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace {

constexpr glm::vec3 world_up = {0.0f, 1.0f, 0.0f};
constexpr float pi_over_2 {static_cast<float>(std::numbers::pi / 2)};
constexpr float orbit_speed {0.01f};
constexpr float pan_speed {0.001f};
constexpr float zoom_speed {0.1f};
constexpr float radius_limit {0.8f};
constexpr float pitch_limit = pi_over_2 - 0.001f;

auto SphericalToVec3(float radius, float phi, float theta) -> glm::vec3;

}

OrbitControls::OrbitControls(PerspectiveCamera* camera) : camera_(camera) {
    using enum MouseEvent::Type;

    mouse_event_listener_ = std::make_shared<EventListener>([this](Event* event) {
        auto e = event->As<MouseEvent>();
        if (!e) return;

        if (e->type == ButtonPressed && curr_button == MouseButton::None) {
            curr_button = e->button;
            prev_pos_ = e->position;
        }

        if (e->type == ButtonReleased && curr_button == e->button) {
            curr_button = MouseButton::None;
        }

        if (e->type == Moved) {
            curr_pos_ = e->position;
            auto offset = curr_pos_ - prev_pos_;

            if (curr_button == MouseButton::Left) {
                phi -= offset.x * orbit_speed;
                theta += offset.y * orbit_speed;
            }

            if (curr_button == MouseButton::Right) {
                const auto speed = pan_speed * radius;
                const glm::vec3 eye = target + SphericalToVec3(radius, phi, theta);
                const glm::vec3 forward = glm::normalize(target - eye);
                const glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
                const glm::vec3 up = glm::normalize(glm::cross(right, forward));

                target -= (right * offset.x - up * offset.y) * speed;
            }
        }

        if (e->type == Scrolled) {
            radius -= zoom_speed * e->scroll.y;
        }

        prev_pos_ = curr_pos_;
    });

    EventDispatcher::Get().AddEventListener("mouse_event", mouse_event_listener_);
}

auto OrbitControls::Update() -> void {
    if (camera_ == nullptr) return;

    theta = std::clamp(theta, -pitch_limit, pitch_limit);
    radius = std::max(radius_limit, radius);

    camera_->transform = glm::lookAt(
        target + SphericalToVec3(radius, phi, theta),
        target,
        glm::vec3 {0.0f, 1.0f, 0.0f}
    );
}

OrbitControls::~OrbitControls() {
    EventDispatcher::Get().RemoveEventListener("mouse_event", mouse_event_listener_);
}

namespace {

auto SphericalToVec3(float radius, float phi, float theta) -> glm::vec3 {
    const auto c = std::cos(theta);
    return glm::vec3 {
        radius * std::sin(phi) * c,
        radius * std::sin(theta),
        radius * std::cos(phi) * c
    };
}

}