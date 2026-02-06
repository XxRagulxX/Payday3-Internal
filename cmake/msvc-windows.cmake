# ============================================================
# Native Windows MSVC toolchain
# ============================================================

# ------------------------------------------------------------
# Target system
# ------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ------------------------------------------------------------
# MSVC is auto-detected by Visual Studio generator
# ------------------------------------------------------------
# DO NOT set CMAKE_C_COMPILER or CMAKE_CXX_COMPILER here

# ------------------------------------------------------------
# HARD disable incremental linking (CRITICAL for injected DLLs)
# Must be CACHE + FORCE and set early
# ------------------------------------------------------------
set(CMAKE_MSVC_INCREMENTAL_LINKING OFF CACHE BOOL "" FORCE)

# ------------------------------------------------------------
# MSVC runtime (static CRT for internals)
# ------------------------------------------------------------
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded CACHE STRING "" FORCE)

# ------------------------------------------------------------
# Prevent CMake try-compile from attempting executables
# (important for DLL-only projects)
# ------------------------------------------------------------
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ------------------------------------------------------------
# Visual Studio specific defaults
# ------------------------------------------------------------
set(CMAKE_VS_GLOBALS
    "UseMultiToolTask=true"
)

message(STATUS "Native MSVC Windows toolchain configured.")