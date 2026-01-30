/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "page_cache.hpp"

#include <cassert>

PageCache::PageCache() {
    free_slots_.reserve(capacity_);
    for (auto y = 0; y < static_cast<int>(kAtlasSlots.y); ++y) {
        for (auto x = 0; x < static_cast<int>(kAtlasSlots.x); ++x) {
            free_slots_.emplace_back(x, y);
        }
    }
}

auto PageCache::Commit(const PageRequest& request, const PageSlot& slot) -> void {
    if (auto already_resident = req_to_slot_.contains(request)) {
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
    if (request.lod >= kMinPinnedLod) return; // no-op for pinned lods
    if (auto it = lru_map_.find(request); it != lru_map_.end()) {
        lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
    }
}

auto PageCache::Acquire(const PageRequest& request) -> ResidencyDecision {
    if (auto it = req_to_slot_.find(request); it != req_to_slot_.end()) {
        return {.slot = it->second, .evicted = std::nullopt};
    }

    if (!free_slots_.empty()) {
        auto slot = free_slots_.back();
        free_slots_.pop_back();
        return {.slot = slot, .evicted = std::nullopt};
    }

    auto it = lru_list_.rbegin();

    while (it != lru_list_.rend()) {
        if (it->lod < kMinPinnedLod) {
            break;
        }
        ++it;
    }

    if (it == lru_list_.rend()) {
        return {.slot = std::nullopt, .evicted = std::nullopt};
    }

    auto evicted_request = *it;

    assert(req_to_slot_.contains(evicted_request));
    auto slot = req_to_slot_.at(evicted_request);

    lru_list_.erase(lru_map_.at(evicted_request));
    lru_map_.erase(evicted_request);
    req_to_slot_.erase(evicted_request);

    return {.slot = slot, .evicted = evicted_request};
}