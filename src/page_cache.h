// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <cassert>
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
    PageCache(int pages_x, int pages_y, unsigned int min_pinned_lod_idx)
      : capacity_(pages_x * pages_y), min_pinned_lod_idx_(min_pinned_lod_idx)
    {
        free_slots_.reserve(capacity_);
        for (auto y = 0; y < pages_y; ++y) {
            for (auto x = 0; x < pages_x; ++x) {
                free_slots_.emplace_back(x, y);
            }
        }
    }

    auto Commit(const PageRequest& request, const PageSlot& slot) {
        auto already_resident = req_to_slot_.contains(request);
        if (already_resident) {
            assert(!already_resident);
            return;
        }

        req_to_slot_[request] = slot;
        lru_list_.emplace_front(request);
        lru_map_[request] = lru_list_.begin();
    }

    auto Touch(const PageRequest& request) {
        if (request.lod >= min_pinned_lod_idx_) return; // no-op for pinned lods
        if (auto it = lru_map_.find(request); it != lru_map_.end()) {
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
        }
    }

    auto Acquire(const PageRequest& request) -> ResidencyDecision {
        if (auto it = req_to_slot_.find(request); it != req_to_slot_.end()) {
            return {.slot = it->second, .evicted = std::nullopt};
        }

        if (free_slots_.empty()) {
            auto it = lru_list_.rbegin();

            while (it != lru_list_.rend()) {
                if (it->lod < min_pinned_lod_idx_) {
                    break;
                }
                ++it;
            }

            if (it == lru_list_.rend()) {
                return {.slot = std::nullopt, .evicted = std::nullopt};
            }

            auto request = *it;

            assert(req_to_slot_.contains(request));
            auto slot = req_to_slot_.at(request);

            lru_list_.erase(lru_map_.at(request));
            lru_map_.erase(request);
            req_to_slot_.erase(request);

            return {.slot = slot, .evicted = request};
        }

        auto slot = free_slots_.back();
        free_slots_.pop_back();

        return {.slot = slot, .evicted = std::nullopt};
    }

    auto Cancel(const PageSlot& slot) -> void {
        free_slots_.emplace_back(slot);
    }

private:
    size_t capacity_;
    unsigned int min_pinned_lod_idx_;

    std::vector<PageSlot> free_slots_ {};
    std::list<PageRequest> lru_list_ {};
    std::unordered_map<PageRequest, std::list<PageRequest>::iterator> lru_map_ {};
    std::unordered_map<PageRequest, PageSlot> req_to_slot_ {};
};