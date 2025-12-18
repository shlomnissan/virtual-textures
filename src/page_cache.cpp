// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "page_cache.h"

#include <cassert>

PageCache::PageCache(const glm::ivec2& slots, unsigned int min_pinned_lod_idx)
  : capacity_(slots.x * slots.y), min_pinned_lod_idx_(min_pinned_lod_idx)
{
    free_slots_.reserve(capacity_);
    for (auto y = 0; y < slots.y; ++y) {
        for (auto x = 0; x < slots.x; ++x) {
            free_slots_.emplace_back(x, y);
        }
    }
}

auto PageCache::Commit(const PageRequest& request, const PageSlot& slot) -> void {
    auto already_resident = req_to_slot_.contains(request);
    if (already_resident) {
        assert(!already_resident);
        return;
    }

    req_to_slot_[request] = slot;
    lru_list_.emplace_front(request);
    lru_map_[request] = lru_list_.begin();
}

auto PageCache::Cancel(const PageSlot& slot) -> void {
    free_slots_.emplace_back(slot);
}

auto PageCache::Touch(const PageRequest& request) -> void {
    if (request.lod >= min_pinned_lod_idx_) return; // no-op for pinned lods
    if (auto it = lru_map_.find(request); it != lru_map_.end()) {
        lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
    }
}

auto PageCache::Acquire(const PageRequest& request) -> ResidencyDecision {
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