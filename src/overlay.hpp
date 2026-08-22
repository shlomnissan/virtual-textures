/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024  - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/vglx.hpp>

class Overlay : public vglx::Scene {
public:
    explicit Overlay(std::shared_ptr<vglx::DynamicTexture2D> tex_atlas);
};
