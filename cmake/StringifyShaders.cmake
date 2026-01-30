file(GLOB_RECURSE SHADERS "**/*.vert" "**/*.frag" "**/*.glsl")

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    get_filename_component(DIRECTORY ${SHADER} DIRECTORY)
    get_filename_component(EXTENSION ${SHADER} EXT)

    string(REGEX REPLACE "\\." "_" EXT ${EXTENSION})
    string(REGEX REPLACE "\\.[^.]*$" "" FILENAME_NO_EXT ${FILENAME})

    set(HEADER_FILE ${DIRECTORY}/headers/${FILENAME_NO_EXT}${EXT}.h)
    set(VAR "_SHADER_${FILENAME_NO_EXT}${EXT}")

    message("🎨 Writing shader ${FILENAME_NO_EXT}.h")

    file(READ ${SHADER} CONTENTS)
    file(WRITE ${HEADER_FILE} "#pragma once\n\nstatic const char* ${VAR} = R\"(\n")
    file(APPEND ${HEADER_FILE} "${CONTENTS}")
    file(APPEND ${HEADER_FILE} "\n)\";")
endforeach()