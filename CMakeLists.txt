cmake_minimum_required(VERSION 3.20)

project(Payday3Internal LANGUAGES CXX C)

# Global configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable incremental linking (critical for injected DLLs)
set(CMAKE_MSVC_INCREMENTAL_LINKING OFF CACHE BOOL "" FORCE)

# Platform / ABI checks
if (NOT WIN32)
    message(FATAL_ERROR "Payday builds Windows DLLs only.")
endif()

if (NOT MSVC)
    message(FATAL_ERROR "MSVC-compatible compiler required (MSVC or clang-cl).")
endif()

# External dependencies (FetchContent)
include(FetchContent)

# Download imgui from GitHub (stable release)
message(STATUS "Downloading imgui v1.92.5 from GitHub...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.5
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/imgui-src
)

# Download minhook from GitHub (stable release)
message(STATUS "Downloading minhook v1.3.4 from GitHub...")
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.4
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/minhook-src
)

# Make dependencies available
FetchContent_MakeAvailable(imgui minhook)


# ImGui sources (DX12 only)
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)

set(IMGUI_HEADERS
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imstb_rectpack.h
    ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.h
)

# MinHook sources
file(GLOB MINHOOK_SOURCES
    ${minhook_SOURCE_DIR}/src/*.c
)
file(GLOB MINHOOK_HDE_SOURCES
    ${minhook_SOURCE_DIR}/src/hde/*.c
)
set(MINHOOK_SOURCES ${MINHOOK_SOURCES} ${MINHOOK_HDE_SOURCES})

# Project sources (excluding external dependencies)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    src/*.h
)

# Remove external dependencies from local sources (we use FetchContent versions)
list(FILTER SOURCES EXCLUDE REGEX "imgui")
list(FILTER SOURCES EXCLUDE REGEX "minhook")
list(FILTER SOURCES EXCLUDE REGEX "external/kiero/minhook")

# Remove missing UE modules
list(FILTER SOURCES EXCLUDE REGEX "WebBrowser")
list(FILTER HEADERS EXCLUDE REGEX "WebBrowser")

# Exclude ALL SDK .cpp files (we'll add back only what we need)
list(FILTER SOURCES EXCLUDE REGEX "SDK/.*\\.cpp$")

# SDK function files we need (add only these back)
set(SDK_FUNCTION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types/gameoffsets/SDK/Basic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types/gameoffsets/SDK/Engine_functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types/gameoffsets/SDK/CoreUObject_functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types/gameoffsets/SDK/Starbreeze_functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types/gameoffsets/SDK/SBZWorldRuntime_functions.cpp
)

# Force MinHook to compile as C
set_source_files_properties(${MINHOOK_SOURCES} PROPERTIES
    LANGUAGE C
    SKIP_PRECOMPILE_HEADERS ON
)

# Create DLL target
add_library(Payday3Internal SHARED
    ${SOURCES}
    ${HEADERS}
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS}
    ${MINHOOK_SOURCES}
    ${SDK_FUNCTION_FILES}
)

# Include directories
target_include_directories(Payday3Internal PRIVATE
    src
    src/external/kiero
    src/types/gameoffsets/SDK
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${minhook_SOURCE_DIR}/include
)

# Preprocessor definitions
target_compile_definitions(Payday3Internal PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    DUMPER7_DISABLE_ASSERTS
)

target_include_directories(Payday3Internal SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/src/types/gameoffsets
)

# Add a fallback for undefined DUMPER7_ASSERTS macros
# This is needed because the SDK generator doesn't generate all assertion macros
add_compile_options(/FI"${CMAKE_CURRENT_SOURCE_DIR}/src/types/gameoffsets/DumperAssertsFix.hpp")

# Compiler flags (MSVC)
target_compile_options(Payday3Internal PRIVATE
    /Zc:__cplusplus
    /permissive-
    /EHsc
    /GR-
    /bigobj
    /W3
    /wd4430  # Disable warning C4430: missing type specifier (for DUMPER7_ASSERTS)
)

# Linker flags
target_link_options(Payday3Internal PRIVATE
    /INCREMENTAL:NO
    /NOLOGO
    /DLL
)

# Windows system libraries (DirectX 12)
target_link_libraries(Payday3Internal PRIVATE
    kernel32
    user32
    gdi32
    psapi
    winmm
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
)

# Output configuration
set_target_properties(Payday3Internal PROPERTIES
    OUTPUT_NAME "Payday3Internal"
    PREFIX ""
    SUFFIX ".dll"
)

# Set output directory
set_target_properties(Payday3Internal PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

#message(STATUS "==============================================")
#message(STATUS "Payday 3 Internal - DX12 Build Configuration")
#message(STATUS "==============================================")
#message(STATUS "ImGui: ${imgui_SOURCE_DIR}")
#message(STATUS "MinHook: ${minhook_SOURCE_DIR}")
#message(STATUS "Output: ${CMAKE_BINARY_DIR}/bin/Payday3Internal.dll")
#message(STATUS "==============================================")