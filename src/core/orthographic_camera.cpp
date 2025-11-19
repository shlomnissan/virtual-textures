// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "orthographic_camera.h"

#include <glm/gtc/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera(
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far
) : left_(left), right_(right), bottom_(bottom), top_(top) {
    projection = glm::ortho(left, right, bottom, top, near, far);
}