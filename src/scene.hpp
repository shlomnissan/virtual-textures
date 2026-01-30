/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <vglx/vglx.hpp>

class Scene : public vglx::Scene {
public:
    Scene(
        std::shared_ptr<vglx::DynamicTexture2D> tex_atlas,
        std::shared_ptr<vglx::DynamicTexture2D> tex_tables
    );

    auto OnAttached(vglx::SharedContextPointer context) -> void override;

    auto OnUpdate(float delta) -> void override;

    auto SetFeedbackMode(bool enabled) -> void;

private:
    vglx::Mesh* terrain_ {nullptr};

    std::shared_ptr<vglx::ShaderMaterial> default_material_ {};
    std::shared_ptr<vglx::ShaderMaterial> feedback_material_ {};
    std::shared_ptr<vglx::DynamicTexture2D> tex_atlas_ {};
};