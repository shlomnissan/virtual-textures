cmake_minimum_required(VERSION 3.20)

project(virtual-textures VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(src)