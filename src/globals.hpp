/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024  - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>
#include <optional>

#include <vglx/vglx.hpp>

inline constexpr auto kWindowWidth {1024};
inline constexpr auto kWindowHeight {768};
inline constexpr auto kSampleCount {4};
inline constexpr auto kLods {5u};
inline constexpr auto kMinPinnedLod {4u};
inline constexpr auto kVirtualSize = vglx::Vector2 {8192.0f, 8192.0f};
inline constexpr auto kAtlasSize = vglx::Vector2 {4096.f, 4096.f};
inline constexpr auto kPageSize = vglx::Vector2 {512.0f, 512.0f};
inline constexpr auto kPagePadding = vglx::Vector2 {4.0f, 4.0f};
inline constexpr auto kSlotSize = kPageSize + kPagePadding;
inline constexpr auto kAtlasSlots = kAtlasSize / kPageSize;

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

struct ResidencyDecision {
    std::optional<PageSlot> slot;
    std::optional<PageRequest> evicted;
};

namespace std {

template<>
struct hash<PageRequest> {
    auto operator()(const PageRequest& r) const noexcept -> size_t {
        auto key =  (uint64_t(r.lod) << 42) ^
                    (uint64_t(uint32_t(r.x)) << 21) ^
                    (uint64_t(uint32_t(r.y)));
        return hash<uint64_t>{}(key);
    }
};

}