// Copyright © 2025 - Present, Shlomi Nissan.
// All rights reserved.

#include "shaders.h"

#include <format>
#include <iostream>
#include <string>

Shaders::Shaders(const std::vector<ShaderInfo>& shaders) {
    program_ = glCreateProgram();

    for (const auto& shader_info : shaders) {
        auto shader_id = glCreateShader(GetShaderType(shader_info.type));
        auto data = shader_info.source.data();

        glShaderSource(shader_id, 1, &data, nullptr);
        glCompileShader(shader_id);

        CheckShaderCompileStatus(shader_id, shader_info.type);

        glAttachShader(program_, shader_id);
        glDeleteShader(shader_id);
    }

    glLinkProgram(program_);
    CheckProgramLinkStatus();
}

auto Shaders::Use() const -> void {
    glUseProgram(program_);
}

auto Shaders::GetShaderType(ShaderType type) const -> unsigned int {
    switch(type) {
        case ShaderType::kVertexShader:
            return GL_VERTEX_SHADER;
        case ShaderType::kFragmentShader:
            return GL_FRAGMENT_SHADER;
        default:
            throw ShaderError {"Unsupported Shader Type"};
    }
}

auto Shaders::GetShaderTypeString(ShaderType type) const -> const char* {
    switch (type) {
        case ShaderType::kVertexShader:
            return "Vertex";
        case ShaderType::kFragmentShader:
            return "Fragment";
        default:
            throw ShaderError {"Unsupported shader type"};
    }
}

auto Shaders::CheckShaderCompileStatus(GLuint shader_id, ShaderType type) const -> void {
    auto success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        auto length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

        auto buffer = std::string{"", static_cast<size_t>(length)};
        glGetShaderInfoLog(shader_id, length, nullptr, buffer.data());

        auto message = std::format(
            "{} shader compilation error\n{}", GetShaderTypeString(type), buffer
        );
        std::cerr << message << '\n';

        throw ShaderError {message};
    }
}

auto Shaders::CheckProgramLinkStatus() const -> void {
    auto success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        auto length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);

        auto buffer = std::string{"", static_cast<size_t>(length)};
        glGetProgramInfoLog(program_, length, nullptr, buffer.data());

        auto message = std::format("Shader program link error:\n{}", buffer);
        std::cerr << message << '\n';

        throw ShaderError {message};
    }
}

auto Shaders::GetUniform(std::string_view name) const -> GLint {
    Use();
    auto loc = glGetUniformLocation(program_, name.data());
    if (loc < 0) {
        throw ShaderError {
            std::format("Uniform '{}' not found", name)
        };
    }
    return loc;
}

auto Shaders::SetUniform(std::string_view uniform, int i) const -> void {
    glUniform1i(GetUniform(uniform), i);
}

auto Shaders::SetUniform(std::string_view uniform, const float f) const -> void {
    glUniform1f(GetUniform(uniform), f);
}

auto Shaders::SetUniform(std::string_view uniform, const glm::vec3& vec) const -> void {
    glUniform3fv(GetUniform(uniform), 1, &vec[0]);
}

auto Shaders::SetUniform(std::string_view uniform, const glm::mat3& matrix) const -> void {
    glUniformMatrix3fv(GetUniform(uniform), 1, GL_FALSE, &matrix[0][0]);
}

auto Shaders::SetUniform(std::string_view uniform, const glm::mat4& matrix) const -> void {
    glUniformMatrix4fv(GetUniform(uniform), 1, GL_FALSE, &matrix[0][0]);
}

Shaders::~Shaders() {
    if (program_) {
        glDeleteProgram(program_);
    }
}