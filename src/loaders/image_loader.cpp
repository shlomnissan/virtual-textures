/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#define STB_IMAGE_IMPLEMENTATION

#include "image_loader.hpp"

#include <cassert>

#include "stb_image.hpp"

ImageLoader::ImageLoader(vglx::LoadScheduler* scheduler) : scheduler_(scheduler) {}

auto ImageLoader::LoadAsync(const fs::path& path) -> ImageLoadHandle {
    assert(scheduler_ != nullptr);

    auto state = std::make_shared<ImageLoadHandle::State>();
    auto handle = ImageLoadHandle {state};

    scheduler_->Enqueue(
        [state, path] {
            stbi_set_flip_vertically_on_load(true);

            auto w = 0;
            auto h = 0;
            auto c = 0;
            unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &c, 4);

            if (data) {
                auto size = static_cast<size_t>(w) * h * 4;
                state->value = std::vector<uint8_t>(data, data + size);
            } else {
                state->error = "Failed to load image: " + path.string();
            }
        },
        [state] {
            assert(state != nullptr);
            state->ready = true;
        }
    );

    return handle;
}