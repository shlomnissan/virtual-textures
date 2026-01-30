find_package(vglx REQUIRED)

include(StringifyShaders)

set(EXECUTABLE ${PROJECT_NAME})

set(SOURCE_CODE
    globals.hpp
    loaders/image_loader.cpp
    loaders/image_loader.hpp
    main.cpp
    page_cache.cpp
    page_cache.hpp
    page_manager.cpp
    page_manager.hpp
    page_tables.cpp
    page_tables.hpp
    scene.cpp
    scene.hpp
)

add_executable(${EXECUTABLE} ${SOURCE_CODE})

target_include_directories(
    ${EXECUTABLE} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(${PROJECT_NAME} PRIVATE vglx::vglx)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:vglx::vglx>
        $<TARGET_FILE_DIR:${EXECUTABLE}>
    )
endif()

add_custom_command(
    TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${EXECUTABLE}>/assets
)