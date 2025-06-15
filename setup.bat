@echo off
REM Set up out-of-source build for OpenGL Renderer

REM Change to script directory
cd /d %~dp0

REM Create build directory if it doesn't exist
if not exist build (
    mkdir build
)

REM Run CMake configuration
cd build
cmake .. -G "Visual Studio 17 2022" -A x64

REM Build the Debug version
cmake --build . --config Debug

REM Path to the built executable
set EXECUTABLE=bin\Debug\opengl-renderer.exe

REM Run the executable if it was built successfully
if exist %EXECUTABLE% (
    echo Running %EXECUTABLE%...
    start "" %EXECUTABLE%
) else (
    echo Failed to build the executable.
)