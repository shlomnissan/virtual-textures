// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include <array>
#include <print>
#include <vector>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "core/orbit_controls.h"
#include "core/orthographic_camera.h"
#include "core/perspective_camera.h"
#include "core/plane_geometry.h"
#include "core/shaders.h"
#include "core/texture2d.h"
#include "core/window.h"

#include "shaders/headers/feedback_frag.h"
#include "shaders/headers/feedback_vert.h"
#include "shaders/headers/flat_frag.h"
#include "shaders/headers/flat_vert.h"
#include "shaders/headers/minimap_frag.h"
#include "shaders/headers/minimap_vert.h"
#include "shaders/headers/page_frag.h"
#include "shaders/headers/page_vert.h"

#include "page_manager.h"
#include "feedback_buffer.h"

constexpr auto window_size = glm::vec2(1024.0f, 1024.0f);
constexpr auto virtual_size = glm::vec2(8192.0f, 8192.0f);
constexpr auto buffer_size = glm::ivec2(window_size / 4.0f);
constexpr auto page_size = glm::vec2(512.0f, 512.0f);
constexpr auto page_padding = glm::vec2(4.0f, 4.0f);
constexpr auto pages = glm::vec2(virtual_size / page_size);

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
        /* bottom= */ 0.0f,
        /* top= */ window_size.y,
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
    controls.radius = 10.0f;

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

    auto page_manager = PageManager {{virtual_size, page_padding, page_size}};
    auto feedback_buffer = FeedbackBuffer {buffer_size};
    auto mip_range = glm::vec2 {0.0f, static_cast<float>(page_manager.LODs() - 1)};

    page_shader.Use();
    page_shader.SetUniform("u_TextureAtlas", 0);
    page_shader.SetUniform("u_PageTable", 1);
    page_shader.SetUniform("u_VirtualSize", virtual_size);
    page_shader.SetUniform("u_PageGrid", pages);
    page_shader.SetUniform("u_AtlasSize", page_manager.AtlasSize());
    page_shader.SetUniform("u_PageSize", page_size);
    page_shader.SetUniform("u_PagePadding", page_padding);
    page_shader.SetUniform("u_MinMaxMipLevel", mip_range);

    auto feedback_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_feedback_vert},
        {ShaderType::kFragmentShader, _SHADER_feedback_frag}
    }};

    feedback_shader.Use();
    feedback_shader.SetUniform("u_VirtualSize", virtual_size);
    feedback_shader.SetUniform("u_PageGrid", pages);
    feedback_shader.SetUniform("u_BufferScreenRatio", 0.25f);
    feedback_shader.SetUniform("u_MinMaxMipLevel", mip_range);

    auto flat_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_flat_vert},
        {ShaderType::kFragmentShader, _SHADER_flat_frag}
    }};

    auto minimap_shader = Shaders {{
        {ShaderType::kVertexShader, _SHADER_minimap_vert},
        {ShaderType::kFragmentShader, _SHADER_minimap_frag}
    }};

    minimap_shader.SetUniform("u_Texture0", 0);

    auto mesh_transform = glm::mat4 {1.0f};
    mesh_transform = glm::scale(mesh_transform, glm::vec3(5.0f, 5.0f, 1.0f));

    const auto feedbackPass = [&]() {
        feedback_buffer.Bind();

        feedback_shader.Use();
        feedback_shader.SetUniform("u_Projection", camera_3d.projection);
        feedback_shader.SetUniform("u_ModelView", camera_3d.transform * mesh_transform);
        geometry.Draw(feedback_shader);

        feedback_buffer.Unbind();
    };

    const auto mainPass = [&]() {
        glViewport(0, 0, window.BufferWidth(), window.BufferHeight());
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        page_manager.BindTexture(PageManager::Texture::Atlas);
        page_manager.BindTexture(PageManager::Texture::PageTables);

        page_shader.Use();
        page_shader.SetUniform("u_Projection", camera_3d.projection);
        page_shader.SetUniform("u_ModelView", camera_3d.transform * mesh_transform);
        geometry.Draw(page_shader);
    };

    const auto minimapPass = [&]() {
        auto border_transform = glm::mat4 {1.0f};
        border_transform = glm::translate(border_transform, glm::vec3(84.0f, 1024.0f - 84.0f, 0.0f));
        border_transform = glm::scale(border_transform, glm::vec3(132.0f, 132.0f, 1.0f));

        flat_shader.Use();
        flat_shader.SetUniform("u_Projection", camera_2d.projection);
        flat_shader.SetUniform("u_ModelView", camera_2d.View() * border_transform);
        geometry.Draw(flat_shader);

        auto minimap_transform = glm::mat4 {1.0f};
        minimap_transform = glm::translate(minimap_transform, glm::vec3(84.0f, 1024.0f - 84.0f, 0.0f));
        minimap_transform = glm::scale(minimap_transform, glm::vec3(128.0f, 128.0f, 1.0f));

        minimap_shader.Use();
        minimap_shader.SetUniform("u_Projection", camera_2d.projection);
        minimap_shader.SetUniform("u_ModelView", camera_2d.View() * minimap_transform);
        geometry.Draw(minimap_shader);
    };

    window.Start([&]([[maybe_unused]] const double dt){
        controls.Update();
        feedbackPass();

        page_manager.FlushUploadQueue();
        page_manager.IngestFeedback(feedback_buffer.Data());
        page_manager.UpdatePageTables();

        mainPass();
        minimapPass();
    });

    return 0;
}