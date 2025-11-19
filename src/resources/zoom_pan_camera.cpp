// Copyright © 2024 - Present, Shlomi Nissan.
// All rights reserved.

#include "zoom_pan_camera.h"

#include <glm/gtc/matrix_transform.hpp>

ZoomPanCamera::ZoomPanCamera(OrthographicCamera* camera) : camera_(camera)  {
    using enum MouseEvent::Type;
    using enum MouseButton;

    mouse_event_listener_ = std::make_shared<EventListener>([this](Event* event) {
        if (auto e = event->As<MouseEvent>()) {
            if ((e->type == ButtonPressed || e->type == ButtonReleased) && e->button == Left) {
                is_panning_ = e->type == ButtonPressed;
                if (!is_panning_) {
                    is_first_pan_ = true;
                }
            }
            if (e->type == Moved) {
                mouse_position_ = e->position;
                if (is_panning_) {
                    pan_ = true;
                }
            }
            if (e->type == Scrolled) {
                curr_scroll_ = e->scroll.y;
                if (curr_scroll_ != 0.0f) zoom_ = true;
            }
        }
    });

    EventDispatcher::Get().AddEventListener("mouse_event", mouse_event_listener_);
}

auto ZoomPanCamera::Pan() -> void {
    pan_ = false;

    if (is_first_pan_) {
        is_first_pan_ = false;
        prev_position_ = mouse_position_;
        return;
    }

    auto delta = (prev_position_ - mouse_position_) * kPanSpeed;
    prev_position_ = mouse_position_;

    camera_->transform = glm::translate(
        camera_->transform,
        glm::vec3 {delta.x, delta.y, 0.0f}
    );
}

auto ZoomPanCamera::Zoom() -> void {
    zoom_ = false;

    auto zoom_factor = 1.0f - curr_scroll_ * 0.01f;
    zoom_factor_ *= zoom_factor;

    if (zoom_factor_ < 0.1f || zoom_factor_ > 5.0f) {
        zoom_factor_ = glm::clamp(zoom_factor_, 0.1f, 5.0f);
        return;
    }

    auto x_offset = camera_->Width() / 2.0f;
    auto y_offset = camera_->Height() / 2.0f;

    camera_->transform = glm::translate(camera_->transform, glm::vec3 {x_offset, y_offset, 0.0f});
    camera_->transform = glm::scale(camera_->transform, glm::vec3 {zoom_factor, zoom_factor, 1.0f});
    camera_->transform = glm::translate(camera_->transform, glm::vec3 {-x_offset, -y_offset, 0.0f});
}

auto ZoomPanCamera::Update() -> void {
    if (zoom_) Zoom();
    if (pan_) Pan();
}

ZoomPanCamera::~ZoomPanCamera() {
    EventDispatcher::Get().RemoveEventListener("mouse_event", mouse_event_listener_);
    mouse_event_listener_.reset();
    camera_ = nullptr;
}