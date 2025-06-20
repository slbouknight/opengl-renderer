cmake_minimum_required(VERSION 3.15)
project(opengl-renderer)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output executable to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define source files (explicitly)
set(SOURCES
    src/main.cpp
    src/Camera.cpp
    src/Shader.cpp
)

# Optional: include headers for IDE visibility (not required for compilation)
set(HEADERS
    include/Camera.h
    include/Shader.h
)

# Define the executable target
add_executable(opengl-renderer ${SOURCES} ${HEADERS})

# Group headers and source files in VS Solution Explorer
source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

# Set startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT opengl-renderer)

# Proper scoped includes (modern CMake best practice)
target_include_directories(opengl-renderer
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

# Link necessary libraries with full paths
target_link_libraries(opengl-renderer
    "${CMAKE_SOURCE_DIR}/lib/glfw3.lib"
    "${CMAKE_SOURCE_DIR}/lib/assimp-vc143-mtd.lib"
    opengl32
)

# Post-build: copy assimp DLL into binary output directory
add_custom_command(TARGET opengl-renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dlls/assimp-vc143-mtd.dll
        $<TARGET_FILE_DIR:opengl-renderer>
)

# Post-build: Copy the entire 'resources' folder to the output directory (bin/)
add_custom_command(TARGET opengl-renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:opengl-renderer>/resources
)

