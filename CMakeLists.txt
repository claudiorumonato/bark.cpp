cmake_minimum_required(VERSION 3.12)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project("bark" C CXX)

set(BARK_BUILD_EXAMPLES ON)

if (NOT XCODE AND NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "${GGML_DIST}/${CMAKE_BUILD_TYPE}")

if (MSVC)
	set(LIB_EXT ".lib")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    endif()
else()
	set(LIB_EXT ".a")
endif()

set(ENCODEC_LIB encodec)

add_library(
    ${ENCODEC_LIB}
    encodec/encodec.cpp
    encodec/encodec.h
    encodec/encoder.h
    encodec/decoder.h
    encodec/quantizer.h
    encodec/ops.cpp
    encodec/ops.h
    encodec/utils.h
    encodec/lstm.h
)

target_link_libraries(${ENCODEC_LIB} PUBLIC ggml)
target_include_directories(${ENCODEC_LIB} PUBLIC .)
target_compile_features(${ENCODEC_LIB} PUBLIC cxx_std_11)

set(BARK_LIB bark)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(BARK_STANDALONE ON)
else()
    set(BARK_STANDALONE OFF)
endif()

option(BUILD_SHARED_LIBS       "bark: build shared libraries" OFF)
option(BARK_BUILD_EXAMPLES     "bark: build examples" ${BARK_STANDALONE})

find_package(ggml REQUIRED)
if (NOT ggml_FOUND)
	message(FATAL_ERROR "System-installed GGML library not found.")
endif()
add_library(ggml ALIAS ggml::ggml)

add_library(${BARK_LIB} STATIC bark.cpp bark.h)

target_link_libraries(${BARK_LIB} PUBLIC ggml encodec)
target_include_directories(${BARK_LIB} PUBLIC . encodec)
target_compile_features(${BARK_LIB} PUBLIC cxx_std_11)

if (BARK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
