# Copyright © 2025 - Present, Shlomi Nissan.
# All rights reserved.

cmake_minimum_required(VERSION 3.22.1)

project(virtual-texture)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE virtual_texture)

message(${CMAKE_SOURCE_DIR}/cmake)

include(cmake/ShaderString.cmake)
ShaderString()

find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(CORE_SOURCES
    src/core/events.h
    src/core/event_dispatcher.h
    src/core/geometry.cpp
    src/core/geometry.h
    src/core/framebuffer.cpp
    src/core/framebuffer.h
    src/core/image.h
    src/core/orthographic_camera.cpp
    src/core/orthographic_camera.h
    src/core/perspective_camera.cpp
    src/core/perspective_camera.h
    src/core/shaders.cpp
    src/core/shaders.h
    src/core/texture2d.cpp
    src/core/texture2d.h
    src/core/timer.h
    src/core/window.cpp
    src/core/window.h
    src/geometries/plane_geometry.cpp
    src/geometries/plane_geometry.h
    src/loaders/image_loader.cpp
    src/loaders/image_loader.h
    src/loaders/loader.h
    src/resources/orbit_controls.cpp
    src/resources/orbit_controls.h
    src/resources/zoom_pan_camera.cpp
    src/resources/zoom_pan_camera.h
)

set(EXTERNAL_SOURCES
    "${CMAKE_SOURCE_DIR}/external/imgui/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/external/imgui/imgui_impl_opengl3.cpp"
)

add_executable(${EXECUTABLE}
    ${LIBS_SOURCES}
    ${CORE_SOURCES}
    ${EXTERNAL_SOURCES}
    src/main.cpp
    src/feedback_buffer.h
    src/page_manager.h
    src/page_table.h
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(${EXECUTABLE} PRIVATE
    glfw
    glad::glad
    glm::glm
    OpenGL::GL
    imgui::imgui
)

add_custom_command(
    TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${EXECUTABLE}>/assets
)