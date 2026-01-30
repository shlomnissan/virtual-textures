/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "scene.hpp"

#include <vglx/imgui/imgui.h>

#include "globals.hpp"

#include "shaders/headers/feedback_frag.h"
#include "shaders/headers/feedback_vert.h"
#include "shaders/headers/material_frag.h"
#include "shaders/headers/material_vert.h"
#include "shaders/headers/sphere_frag.h"
#include "shaders/headers/sphere_vert.h"

namespace {

auto mesh_handle = vglx::MeshLoadHandle {};

}

Scene::Scene(
    std::shared_ptr<vglx::DynamicTexture2D> tex_atlas,
    std::shared_ptr<vglx::DynamicTexture2D> tex_tables
) : tex_atlas_(tex_atlas) {
    default_material_ = vglx::ShaderMaterial::Create({
        _SHADER_material_vert,
        _SHADER_material_frag,
        {},
        {
            {"u_TextureAtlas", tex_atlas},
            {"u_PageTable", tex_tables}
        }
    });

    feedback_material_ = vglx::ShaderMaterial::Create({
        _SHADER_feedback_vert,
        _SHADER_feedback_frag,
        {
            {"u_VirtualSize",  kVirtualSize},
            {"u_PageGrid", kVirtualSize / kPageSize},
            {"u_MinMaxMipLevel", vglx::Vector2 {0.0f, static_cast<float>(kLods) - 1}},
            {"u_BufferScreenRatio", 0.25f}
        }
    });

    Add(vglx::Mesh::Create(
        vglx::SphereGeometry::Create({.radius = 500.0f}),
        vglx::ShaderMaterial::Create({_SHADER_sphere_vert, _SHADER_sphere_frag})
    ))->GetMaterial()->two_sided = true;
}

auto Scene::OnAttached(vglx::SharedContextPointer context) -> void {
    Add(vglx::OrbitControls::Create(context->camera, {
        .radius = 350.0f,
        .pitch = vglx::math::DegToRad(20.0f),
        .yaw = vglx::math::DegToRad(30.0f),
    }));

    mesh_handle = context->mesh_loader->LoadAsync("assets/snowy_mountain.msh");
}

auto Scene::OnUpdate([[maybe_unused]] float delta) -> void {
    if (auto result = mesh_handle.TryTake()) {
        terrain_ = static_cast<vglx::Mesh*>(result.value()->Children().front().get());
        terrain_->SetMaterial(default_material_);
        terrain_->GetMaterial()->two_sided = true;
        terrain_->SetScale(30.0f);
        Add(std::move(result.value()));
    }

    if (tex_atlas_->renderer_id == 0) return;

    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive));
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(144, 163), ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

    if (ImGui::Begin("Texture Atlas", nullptr, window_flags)) {
        ImGui::Image(
            (ImTextureID)(intptr_t)tex_atlas_->renderer_id,
            ImVec2(128, 128),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    }

    ImGui::End();

    ImGui::PopStyleColor();
}

auto Scene::SetFeedbackMode(bool enabled) -> void {
    if (terrain_ == nullptr) return;

    if (enabled) {
        terrain_->SetMaterial(feedback_material_);
    } else {
        terrain_->SetMaterial(default_material_);
    }
}