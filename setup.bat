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

REM Change to the output directory
cd bin\Debug

REM Run the executable from this working directory
echo Running opengl-renderer.exe from %CD%...
opengl-renderer.exe
