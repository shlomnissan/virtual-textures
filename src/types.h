// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include <cstdint>

struct PageSlot {
    int x;
    int y;
    auto operator<=>(const PageSlot&) const = default;
};

struct PageRequest {
    uint32_t lod;
    int x;
    int y;
    auto operator<=>(const PageRequest&) const = default;
};

namespace std {

template<>
struct hash<PageRequest> {
    auto operator()(const PageRequest& r) const noexcept -> size_t {
        auto key =  (uint64_t(r.lod) << 42) ^
                    (uint64_t(uint32_t(r.x)) << 21) ^
                    (uint64_t(uint32_t(r.y)));
        return std::hash<uint64_t>{}(key);
    }
};

}
