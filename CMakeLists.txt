cmake_minimum_required(VERSION 3.25)

project(virtual-textures VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(FetchContent)
FetchContent_Declare(
    vglx
    GIT_REPOSITORY https://github.com/shlomnissan/vglx.git
    GIT_TAG v0.4.0
    GIT_SHALLOW TRUE
)
set(VGLX_BUILD_IMGUI ON)
FetchContent_MakeAvailable(vglx)

add_subdirectory(src)
