// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include <array>
#include <print>
#include <vector>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "core/orthographic_camera.h"
#include "core/perspective_camera.h"
#include "core/shaders.h"
#include "core/window.h"
#include "core/texture2d.h"
#include "geometries/plane_geometry.h"
#include "resources/orbit_controls.h"
#include "shaders/headers/feedback_vert.h"
#include "shaders/headers/feedback_frag.h"
#include "shaders/headers/page_vert.h"
#include "shaders/headers/page_frag.h"
#include "shaders/headers/minimap_vert.h"
#include "shaders/headers/minimap_frag.h"

#include "page_manager.h"

constexpr auto window_size = glm::vec2(1024.0f, 1024.0f);

auto main() -> int {
    std::shared_ptr<ImageLoader> loader_;

    auto window = Window {
        static_cast<int>(window_size.x),
        static_cast<int>(window_size.y),
        "Virtual Texture"
    };

    auto camera_2d = OrthographicCamera {
        /* left= */ 0.0f,
        /* right= */ window_size.x,
        /* bottom= */ window_size.y,
        /* top= */ 0.0f,
        /* near= */ -1.0f,
        /* far= */ 1.0f
    };

    auto camera_3d = PerspectiveCamera {
        /* fov= */ 60.0f,
        /* aspect= */ window_size.x / window_size.y,
        /* near= */ 0.1f,
        /* far= */ 1000.0f
    };

    auto controls = OrbitControls {&camera_3d};
    controls.radius = 2.0f;

    const auto geometry = PlaneGeometry {{
        .width = 1.0f,
        .height = 1.0f,
        .width_segments = 1,
        .height_segments = 1
    }};

    auto page_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_page_vert},
        {ShaderType::kFragmentShader, _SHADER_page_frag}
    }};

    auto minimap_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_minimap_vert},
        {ShaderType::kFragmentShader, _SHADER_minimap_frag}
    }};

    auto page_manager = PageManager {};

    const auto mainPass = [&]() {
        glViewport(0, 0, window.BufferWidth(), window.BufferHeight());
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        page_shader.Use();
        page_shader.SetUniform("u_Projection", camera_3d.projection);
        page_shader.SetUniform("u_ModelView", camera_3d.transform);
        geometry.Draw(page_shader);

        auto minimap_model = glm::mat4 {1.0f};
        minimap_model = glm::translate(minimap_model, glm::vec3(120.0f, 120.0f, 0.0f));
        minimap_model = glm::scale(minimap_model, glm::vec3(200.0f, 200.0f, 1.0f));

        page_manager.atlas.Bind();

        minimap_shader.Use();
        minimap_shader.SetUniform("u_Projection", camera_2d.projection);
        minimap_shader.SetUniform("u_ModelView", camera_2d.View() * minimap_model);
        geometry.Draw(minimap_shader);
    };

    window.Start([&]([[maybe_unused]] const double dt){
        controls.Update(static_cast<float>(dt));
        mainPass();
    });

    return 0;
}