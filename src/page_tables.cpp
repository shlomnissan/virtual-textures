/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "page_tables.hpp"

#include <span>

namespace {

constexpr auto pages =  kVirtualSize / kPageSize;
constexpr auto pages_x = static_cast<int>(pages.x);
constexpr auto pages_y = static_cast<int>(pages.y);

auto get_page_index(int lod, int page_x, int page_y) -> size_t {
    const auto row_width = pages_x >> lod;
    return static_cast<size_t>(page_y) * row_width + page_x;
}

}

PageTables::PageTables() {
    auto x = pages_x;
    auto y = pages_y;
    for (auto i = 0; i < kLods; ++i) {
        tables_.emplace_back(x * y, 0u);
        if (x == 1 && y == 1) break;
        x = std::max(x >> 1, 1);
        y = std::max(y >> 1, 1);
    }

    tex_tables_ = vglx::DynamicTexture2D::Create({
        .width = pages_x,
        .height = pages_y,
        .mips = static_cast<unsigned>(tables_.size()),
        .format = vglx::Texture::Format::R32UI,
        .color_space = vglx::Texture::ColorSpace::Linear
    });
}

auto PageTables::Write(const PageRequest& request, uint32_t entry) -> void {
    if (request.lod >= tables_.size()) return;

    const auto idx = get_page_index(request.lod, request.x, request.y);
    tables_[request.lod][idx] = entry;
}

auto PageTables::SyncTables() -> void {
    for (size_t i = 0; i < tables_.size(); ++i) {
        auto bytes = std::span<const std::uint8_t>(
            reinterpret_cast<const std::uint8_t*>(tables_[i].data()),
            tables_[i].size() * sizeof(std::uint32_t)
        );

        tex_tables_->UpdateSubregion(
            static_cast<int>(i),
            0,
            0,
            std::max(1, pages_x >> i),
            std::max(1, pages_y >> i),
            bytes
        );
    }
}

auto PageTables::IsResident(const PageRequest& request) const -> bool {
    const auto idx = get_page_index(request.lod, request.x, request.y);
    return static_cast<bool>(tables_[request.lod][idx] & 1);
}