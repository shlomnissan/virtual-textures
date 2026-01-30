/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024  - Present, Shlomi Nissan
===========================================================================
*/

#include <print>

#include <vglx/vglx.hpp>

#include "globals.hpp"
#include "page_manager.hpp"

#include "scene.hpp"

#include "loaders/image_loader.hpp"

auto main() -> int {
    auto window = vglx::Window {{
        .title = "Virtual Textures",
        .width = kWindowWidth,
        .height = kWindowHeight,
        .sample_count = kSampleCount,
        .vsync = false
    }};

    if (auto result = window.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto renderer = vglx::Renderer {{
        .framebuffer_width = window.FramebufferWidth(),
        .framebuffer_height = window.FramebufferHeight(),
        .sample_count = kSampleCount,
        .clear_color = 0x000000
    }};

    if (auto result = renderer.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto feedback_target = vglx::RenderTarget::Create({
        .width = kWindowWidth / 4,
        .height = kWindowHeight / 4,
        .format = vglx::Texture::Format::R32UI,
        .has_depth = true,
        .enable_readback = true
    });

    auto camera = vglx::PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = window.AspectRatio(),
        .near = 0.1f,
        .far = 1000.0f
    });

    auto context = vglx::SharedContext::Create(&window, camera.get());
    auto image_loader = ImageLoader {context->load_scheduler.get()};
    auto page_manager = PageManager {&image_loader};
    auto timer = vglx::FrameTimer {true};

    auto scene = std::make_unique<Scene>(
        page_manager.GetAtlasTexture(),
        page_manager.GetPageTablesTexture()
    );
    scene->SetContext(context.get());

    while(!window.ShouldClose()) {
        window.PollEvents();
        window.BeginUIFrame();

        context->load_scheduler->Pump();

        page_manager.FlushProcessingRequests();

        scene->Advance(timer.Tick());
        scene->SetFeedbackMode(true);
        renderer.Render(scene.get(), camera.get(), feedback_target.get());

        auto data = feedback_target->ReadColorData();
        const auto pixel_count = data.size() / sizeof(std::uint32_t);
        auto pixels = std::span<const std::uint32_t>(
            reinterpret_cast<const std::uint32_t*>(data.data()),
            pixel_count
        );

        page_manager.IngestFeedback(pixels);
        scene->SetFeedbackMode(false);
        renderer.Render(scene.get(), camera.get());

        window.EndUIFrame();
        window.SwapBuffers();
    }

    return 0;
}