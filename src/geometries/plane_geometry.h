// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#pragma once

#include "core/geometry.h"

class PlaneGeometry : public Geometry {
public:
    struct Parameters {
        float width;
        float height;
        unsigned width_segments;
        unsigned height_segments;
    };

    explicit PlaneGeometry(const Parameters& params);
};