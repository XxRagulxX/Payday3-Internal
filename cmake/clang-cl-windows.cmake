# ============================================================
# Windows DLL cross-compilation from Linux
# clang-cl + MSVC ABI via msvc-wine
# ============================================================

# ------------------------------------------------------------
# Host guard
# ------------------------------------------------------------
if(WIN32 OR CYGWIN)
    message(FATAL_ERROR "This toolchain is for Linux hosts only.")
endif()

# ------------------------------------------------------------
# Target system (MUST be first)
# ------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# THIS IS THE FIX
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# ------------------------------------------------------------
# HARD disable MSVC incremental linking
# MUST be CACHE + FORCE and BEFORE compiler checks
# ------------------------------------------------------------
set(CMAKE_MSVC_INCREMENTAL_LINKING OFF CACHE BOOL "" FORCE)

message(STATUS "Configuring Windows DLL cross-compilation (clang-cl / MSVC ABI)")

# ------------------------------------------------------------
# Locate clang-cl
# ------------------------------------------------------------
find_program(CLANG_CL_EXECUTABLE clang-cl)
if(NOT CLANG_CL_EXECUTABLE)
    message(FATAL_ERROR "clang-cl not found. Install LLVM.")
endif()

set(CMAKE_C_COMPILER   ${CLANG_CL_EXECUTABLE} CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER ${CLANG_CL_EXECUTABLE} CACHE STRING "" FORCE)
set(CMAKE_LINKER       lld-link              CACHE STRING "" FORCE)

set(CMAKE_C_COMPILER_TARGET   x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

# ------------------------------------------------------------
# MSVC runtime (static CRT for internals)
# ------------------------------------------------------------
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded CACHE STRING "" FORCE)

# ------------------------------------------------------------
# msvc-wine root
# ------------------------------------------------------------
set(MSVC_WINE_ROOT "$ENV{HOME}/my_msvc/opt/msvc")

if(NOT EXISTS "${MSVC_WINE_ROOT}/VC/Tools/MSVC")
    message(FATAL_ERROR
        "msvc-wine not found or incomplete at:\n"
        "  ${MSVC_WINE_ROOT}"
    )
endif()

message(STATUS "Using msvc-wine at: ${MSVC_WINE_ROOT}")

# ------------------------------------------------------------
# Versions (match your install)
# ------------------------------------------------------------
set(MSVC_VERSION   "14.44.35207")
set(WINSDK_VERSION "10.0.26100.0")

# ------------------------------------------------------------
# Inject MSVC + Windows SDK paths (CORRECT)
# ------------------------------------------------------------
string(JOIN ";" MSVC_LIB_PATHS
    "${MSVC_WINE_ROOT}/VC/Tools/MSVC/${MSVC_VERSION}/lib/x64"
    "${MSVC_WINE_ROOT}/Windows Kits/10/Lib/${WINSDK_VERSION}/um/x64"
    "${MSVC_WINE_ROOT}/Windows Kits/10/Lib/${WINSDK_VERSION}/ucrt/x64"
)

string(JOIN ";" MSVC_INCLUDE_PATHS
    "${MSVC_WINE_ROOT}/VC/Tools/MSVC/${MSVC_VERSION}/include"
    "${MSVC_WINE_ROOT}/Windows Kits/10/Include/${WINSDK_VERSION}/ucrt"
    "${MSVC_WINE_ROOT}/Windows Kits/10/Include/${WINSDK_VERSION}/um"
    "${MSVC_WINE_ROOT}/Windows Kits/10/Include/${WINSDK_VERSION}/shared"
)

set(ENV{LIB}     "${MSVC_LIB_PATHS}")
set(ENV{INCLUDE} "${MSVC_INCLUDE_PATHS}")
set(ENV{PATH}    "${MSVC_WINE_ROOT}/bin/x64:$ENV{PATH}")

# ------------------------------------------------------------
# Disable non-MSVC discovery
# ------------------------------------------------------------
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)

# ------------------------------------------------------------
# Compiler flags (Unreal-internal safe)
# ------------------------------------------------------------
add_compile_options(
    /std:c++20
    /Zc:__cplusplus
    /permissive-
    /EHsc-
    /GR-
    /GS-
)

# ------------------------------------------------------------
# Linker flags
# (INCREMENTAL already disabled globally above)
# ------------------------------------------------------------
add_link_options(
    /NOLOGO
    /SUBSYSTEM:WINDOWS
)

message(STATUS "clang-cl Windows DLL toolchain configured successfully.")