@echo off

set "BUILD_TYPE=%1"
set "GGML_DIR=D:/libs/dist/%BUILD_TYPE%/lib/cmake/ggml"
set "ENCODEC_DIR=D:/Build/encodec.cpp/build/%BUILD_TYPE%"

echo -------------------------------------------------------------------------------
echo -------------------------------------------------------------------------------
echo -------------------------------------------------------------------------------

cmake -S . -B build -G "Visual Studio 17 2022" -A x64 "-Dggml_DIR=%GGML_DIR%" "-Dencodec_BUILD=%ENCODEC_DIR%" --fresh
cmake --build build --config %BUILD_TYPE% -j --clean-first
