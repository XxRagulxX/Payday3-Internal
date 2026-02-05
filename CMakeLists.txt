cmake_minimum_required(VERSION 3.20)

project(UnrealSDKBase LANGUAGES CXX C)

# ==================================================
# Global configuration
# ==================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable incremental linking (critical for injected DLLs)
set(CMAKE_MSVC_INCREMENTAL_LINKING OFF CACHE BOOL "" FORCE)

# ==================================================
# Platform / ABI checks
# ==================================================
if (NOT WIN32)
    message(FATAL_ERROR "UnrealSDKBase builds Windows DLLs only.")
endif()

# MSVC ABI required (MSVC or clang-cl)
if (NOT MSVC)
    message(FATAL_ERROR "MSVC-compatible compiler required (MSVC or clang-cl).")
endif()

# ==================================================
# Project sources
# ==================================================
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    UnrealSDKBase/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    UnrealSDKBase/*.h
)

# Remove missing UE modules (example: WebBrowser)
list(FILTER SOURCES EXCLUDE REGEX "WebBrowser")
list(FILTER HEADERS EXCLUDE REGEX "WebBrowser")

# ==================================================
# MinHook (required by kiero)
# ==================================================
file(GLOB MINHOOK_SOURCES
    UnrealSDKBase/kiero/minhook/src/*.c
    UnrealSDKBase/kiero/minhook/src/hde/*.c
)

# Force MinHook to compile as C and skip C++ PCH
set_source_files_properties(${MINHOOK_SOURCES} PROPERTIES
    LANGUAGE C
    SKIP_PRECOMPILE_HEADERS ON
)

# Force MinHook to compile as C (IMPORTANT)
set_source_files_properties(${MINHOOK_SOURCES} PROPERTIES
    LANGUAGE C
)

# ==================================================
# Library target (DLL)
# ==================================================
add_library(UnrealSDKBase SHARED
    ${SOURCES}
    ${HEADERS}
    ${MINHOOK_SOURCES}
)

# ==================================================
# Include directories
# ==================================================
target_include_directories(UnrealSDKBase PRIVATE
    UnrealSDKBase
    UnrealSDKBase/imgui
    UnrealSDKBase/kiero
    UnrealSDKBase/kiero/minhook/include
    UnrealSDKBase/SDK
)

# ==================================================
# Precompiled headers
# ==================================================
target_precompile_headers(UnrealSDKBase PRIVATE
    UnrealSDKBase/pch.h
)

# ==================================================
# Preprocessor definitions
# ==================================================
target_compile_definitions(UnrealSDKBase PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
)

# ==================================================
# Compiler flags (MSVC ABI safe)
# ==================================================
target_compile_options(UnrealSDKBase PRIVATE
    /std:c++20
    /Zc:__cplusplus
    /permissive-
    /EHsc
    /GR-
    /bigobj
)

# ==================================================
# Linker flags
# ==================================================
target_link_options(UnrealSDKBase PRIVATE
    /INCREMENTAL:NO
    /NOLOGO
)

# ==================================================
# Windows system libraries
# ==================================================
target_link_libraries(UnrealSDKBase PRIVATE
    kernel32
    user32
    gdi32
    psapi
    winmm
)

# ==================================================
# Output name
# ==================================================
set_target_properties(UnrealSDKBase PROPERTIES
    OUTPUT_NAME "UnrealSDKBase"
)