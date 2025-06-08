cmake_minimum_required(VERSION 3.15)
project(opengl-renderer)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include and lib paths
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)
link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

# Source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Executable target
add_executable(opengl-renderer
    src/main.cpp
    src/Camera.cpp
)
set_target_properties(opengl-renderer PROPERTIES WIN32_EXECUTABLE FALSE)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT opengl-renderer)

# Link static libraries
target_link_libraries(opengl-renderer
    glfw3
    assimp-vc143-mtd
    opengl32
)

# Post-build DLL copy (Windows)
add_custom_command(TARGET opengl-renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dlls/assimp-vc143-mtd.dll
        $<TARGET_FILE_DIR:opengl-renderer>)