/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <list>
#include <optional>
#include <unordered_map>
#include <vector>

#include "globals.hpp"

class PageCache {
public:
    PageCache();

    auto Commit(const PageRequest& request, const PageSlot& slot) -> void;

    auto Cancel(const PageSlot& slot) -> void;

    auto Touch(const PageRequest& request) -> void;

    [[nodiscard]] auto Acquire(const PageRequest& request) -> ResidencyDecision;

private:
    std::vector<PageSlot> free_slots_ {};

    std::list<PageRequest> lru_list_ {};

    std::unordered_map<PageRequest, std::list<PageRequest>::iterator> lru_map_ {};
    std::unordered_map<PageRequest, PageSlot> req_to_slot_ {};

    size_t capacity_ = static_cast<size_t>(kAtlasSlots.x * kAtlasSlots.y);
};