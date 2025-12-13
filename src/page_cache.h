// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <list>
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

    auto Commit(const PageRequest& request, const PageSlot& slot) {
        if (req_to_slot_.contains(request)) {
            // add assert, this shouldn't happen
            return;
        }

        req_to_slot_[request] = slot;
        lru_list_.emplace_front(request);
        lru_map_[request] = lru_list_.begin();
    }

    auto Touch(const PageRequest& request) {
        if (auto it = lru_map_.find(request); it != lru_map_.end()) {
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
        }
    }

    auto Acquire(const PageRequest& request) -> ResidencyDecision {
        // if already resident (req_to_slot)
            // return the slot

        // add assert no way for lru and free slots to be empty

        if (free_slots_.empty()) {
            // get request from back of lru_list_
            // get slot from req_to_slot
            // if we have both
                // pop back lru_list_
                // erase value from lru_map_
                // erase value from req_to_slot
            // return residency decision
                // slot: slot
                // evicted: request
            // note: the page manager is responsible for updating the page
            //   table when it flushes the upload queue. if evicted exists
            //   the page manager can clear the table from RequestPage
            return ResidencyDecision {std::nullopt, std::nullopt};
        }

        auto slot = free_slots_.back();
        free_slots_.pop_back();

        return ResidencyDecision {slot, std::nullopt};
    }

    auto Cancel(const PageSlot& slot) -> void {
        free_slots_.emplace_back(slot);
    }

private:
    size_t capacity_ {0};

    std::vector<PageSlot> free_slots_ {};
    std::list<PageRequest> lru_list_ {};
    std::unordered_map<PageRequest, std::list<PageRequest>::iterator> lru_map_ {};
    std::unordered_map<PageRequest, PageSlot> req_to_slot_ {};
};