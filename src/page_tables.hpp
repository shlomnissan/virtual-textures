/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <vglx/vglx.hpp>

#include "globals.hpp"

class PageTables {
public:
    PageTables();

    auto Write(const PageRequest& request, uint32_t entry) -> void;

    auto SyncTables() -> void;

    [[nodiscard]] auto IsResident(const PageRequest& request) const -> bool;

private:
    friend class PageManager;

    std::shared_ptr<vglx::DynamicTexture2D> tex_tables_ {nullptr};

    std::vector<std::vector<uint32_t>> tables_ {};
};