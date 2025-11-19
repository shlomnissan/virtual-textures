// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <type_traits>

#include <glm/vec2.hpp>

struct Event {
    template<class T> requires std::is_base_of_v<Event, T>
    [[nodiscard]] auto Is() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }

    template<class T> requires std::is_base_of_v<Event, T>
    [[nodiscard]] auto As() {
        return dynamic_cast<T*>(this);
    }

    template<class T> requires std::is_base_of_v<Event, T>
    [[nodiscard]] auto As() const {
        return dynamic_cast<const T*>(this);
    }

    virtual ~Event() = default;
};

enum class MouseButton {
    None,
    Left,
    Right,
    Middle
};

struct MouseEvent : public Event {
    enum class Type {
        Moved,
        ButtonPressed,
        ButtonReleased,
        Scrolled
    };

    glm::vec2 position;
    glm::vec2 scroll;

    MouseEvent::Type type;
    MouseButton button;
};