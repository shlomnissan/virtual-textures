// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "types.h"

struct ResidencyDecision {
    std::optional<PageSlot> slot;
    std::optional<PageRequest> evicted;
};

class PageCache {
public:
    PageCache(int pages_x, int pages_y) : capacity_(pages_x * pages_y) {
        free_slots_.reserve(capacity_);
        for (auto y = 0; y < pages_y; ++y) {
            for (auto x = 0; x < pages_x; ++x) {
                free_slots_.emplace_back(x, y);
            }
        }
    }

    auto Alloc(const PageRequest& request) -> ResidencyDecision {
        if (free_slots_.empty()) {
            return ResidencyDecision {std::nullopt, std::nullopt};
        }

        auto slot = free_slots_.back();
        free_slots_.pop_back();

        return ResidencyDecision {slot, std::nullopt};
    }

private:
    size_t capacity_ {0};

    std::vector<PageSlot> free_slots_ {};
};