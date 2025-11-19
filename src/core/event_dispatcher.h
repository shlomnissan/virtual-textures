// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include "events.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using EventListener = std::function<void(Event*)>;

class EventDispatcher {
public:
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    static auto Get() -> EventDispatcher& {
        static auto instance = EventDispatcher {};
        return instance;
    }

    auto AddEventListener(const std::string& name, std::weak_ptr<EventListener> listener) {
        if (callbacks_.contains(name)) {
            callbacks_.at(name).emplace_back(listener);
        } else {
            callbacks_.try_emplace(name, std::vector{listener});
        }
    }

    auto RemoveEventListener(const std::string& name, std::shared_ptr<EventListener> listener) {
        if (callbacks_.contains(name)) {
            auto& callbacks = callbacks_[name];
            std::erase_if(callbacks, [&listener](const auto& callback) {
                if (auto c = callback.lock()) return c == listener;
                return false;
            });
        }
    }

    auto Dispatch(const std::string& name, std::unique_ptr<Event> event) {
        if (!callbacks_.contains(name)) {
            return;
        }
        auto& callbacks = callbacks_[name];
        for (auto iter = begin(callbacks); iter != end(callbacks);) {
            if (const auto& callback = iter->lock()) {
                (*callback)(event.get());
                iter++;
            } else {
                iter = callbacks.erase(iter); // update iter to the next valid position
            }
        }
    }

private:
    EventDispatcher() = default;
    ~EventDispatcher() = default;

    std::unordered_map<std::string, std::vector<std::weak_ptr<EventListener>>> callbacks_;
};