// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "perspective_camera.h"

#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(
    float fov,
    float aspect,
    float near,
    float far
) {
    projection_ = glm::perspective(
        glm::radians(fov),
        aspect,
        near,
        far
    );
}