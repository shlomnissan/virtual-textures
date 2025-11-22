// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include <array>
#include <print>
#include <vector>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "types.h"

#include "core/framebuffer.h"
#include "core/orthographic_camera.h"
#include "core/shaders.h"
#include "core/texture2d.h"
#include "core/window.h"
#include "geometries/plane_geometry.h"
#include "resources/zoom_pan_camera.h"
#include "shaders/headers/feedback_vert.h"
#include "shaders/headers/feedback_frag.h"
#include "shaders/headers/page_vert.h"
#include "shaders/headers/page_frag.h"

#include "page_manager.h"

constexpr auto window_size = 1024.0f;
constexpr auto buffer_size = 256.0f;
constexpr auto page_size = 1024.0f;
constexpr auto image_size = 8192.0f;
constexpr auto lods = 4;

auto main() -> int {
    auto window = Window {
        static_cast<int>(window_size),
        static_cast<int>(window_size),
        "Virtual Textures"
    };

    auto camera = OrthographicCamera {0.0f, window_size, window_size, 0.0f, -1.0f, 1.0f};
    auto controls = ZoomPanCamera {&camera};

    const auto geometry = PlaneGeometry {{
        .width = page_size,
        .height = page_size,
        .width_segments = 1,
        .height_segments = 1
    }};

    auto feedback_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_feedback_vert},
        {ShaderType::kFragmentShader, _SHADER_feedback_frag}
    }};

    feedback_shader.Use();
    feedback_shader.SetUniform("u_TextureSize", image_size);
    feedback_shader.SetUniform("u_PageSize", page_size);
    feedback_shader.SetUniform("u_BufferScreenRatio", buffer_size / window_size);
    feedback_shader.SetUniform("u_MaxMipLevel", lods - 1);

    auto page_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_page_vert},
        {ShaderType::kFragmentShader, _SHADER_page_frag}
    }};

    auto page_manager = PageManager {image_size, page_size, lods};

    auto feedback_buffer = Framebuffer {
        static_cast<int>(buffer_size),
        static_cast<int>(buffer_size)
    };

    auto feedback_texture = Texture2D {};
    feedback_texture.InitTexture(
        feedback_buffer.Width(),
        feedback_buffer.Height(),
        GL_R32UI,
        GL_RED_INTEGER,
        GL_UNSIGNED_INT,
        nullptr
    );

    feedback_buffer.AddColorAttachment(feedback_texture.Id());
    if (!feedback_buffer.IsComplete()) {
        std::println("Failed to create a framebuffer");
        return 1;
    }

    const auto feedbackPass = [&]() {
        feedback_buffer.Bind();
        glViewport(0, 0, feedback_buffer.Width(), feedback_buffer.Height());

        const auto clear_value = std::array<GLuint, 4> {0xFFFFFFFFu, 0, 0, 0};
        glClearBufferuiv(GL_COLOR, 0, clear_value.data());

        feedback_shader.Use();
        feedback_shader.SetUniform("u_Projection", camera.projection);

        auto offset = page_size / 2;
        auto model = glm::translate(glm::mat4(1.0f), glm::vec3 {offset, offset, 0.0f});

        feedback_shader.SetUniform("u_ModelView", camera.View() * model);
        geometry.Draw(feedback_shader);

        feedback_buffer.Unbind();
    };

    const auto mainPass = [&]() {
        glViewport(0, 0, window.BufferWidth(), window.BufferHeight());
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        page_shader.Use();
        page_shader.SetUniform("u_Projection", camera.projection);

        auto pages = page_manager.GetVisiblePages();
        auto visible = pages.size();

        for (auto& page : pages) {
            if (page->state == PageState::Loaded) {
                page->texture.Bind();
                page_shader.SetUniform("u_ModelView", camera.View() * page->Transform());
                geometry.Draw(page_shader);
            }
        }
    };

    auto page_data = std::vector<GLuint>(
        feedback_texture.Width() * feedback_texture.Height()
    );

    window.Start([&]([[maybe_unused]] const double _){
        controls.Update();

        feedbackPass();
        feedback_texture.Read(page_data.data());
        page_manager.IngestPages(page_data);

        mainPass();

        page_manager.Debug(camera);
    });

    return 0;
}