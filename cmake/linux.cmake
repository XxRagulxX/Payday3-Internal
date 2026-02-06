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

# Prevent CMake from trying to build executables during compiler tests
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ------------------------------------------------------------
# HARD disable MSVC incremental linking
# MUST be CACHE + FORCE and BEFORE compiler checks
# ------------------------------------------------------------
set(CMAKE_MSVC_INCREMENTAL_LINKING OFF CACHE BOOL "" FORCE)

# Override CMake's default linker flags to remove /INCREMENTAL:YES
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG_INIT "/DEBUG /INCREMENTAL:NO" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE_INIT "/INCREMENTAL:NO" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO_INIT "/DEBUG /INCREMENTAL:NO" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL_INIT "/INCREMENTAL:NO" CACHE STRING "" FORCE)

message(STATUS "Configuring Windows DLL cross-compilation (clang-cl / MSVC ABI)")

# ------------------------------------------------------------
# Locate clang-cl and lld-link
# ------------------------------------------------------------
find_program(CLANG_CL_EXECUTABLE clang-cl)
if(NOT CLANG_CL_EXECUTABLE)
    message(FATAL_ERROR "clang-cl not found. Install LLVM with clang-cl support.")
endif()

find_program(LLD_LINK_EXECUTABLE lld-link)
if(NOT LLD_LINK_EXECUTABLE)
    message(FATAL_ERROR "lld-link not found. Install LLVM with lld support.")
endif()

set(CMAKE_C_COMPILER   ${CLANG_CL_EXECUTABLE} CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER ${CLANG_CL_EXECUTABLE} CACHE STRING "" FORCE)
set(CMAKE_LINKER       ${LLD_LINK_EXECUTABLE} CACHE STRING "" FORCE)
set(CMAKE_RC_COMPILER  llvm-rc               CACHE STRING "" FORCE)

set(CMAKE_C_COMPILER_TARGET   x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

# ------------------------------------------------------------
# MSVC runtime (static CRT for internals)
# ------------------------------------------------------------
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded CACHE STRING "" FORCE)

# ------------------------------------------------------------
# msvc-wine root (adjust path as needed)
# ------------------------------------------------------------
set(MSVC_WINE_ROOT "$ENV{HOME}/my_msvc/opt/msvc" CACHE PATH "Path to msvc-wine installation")

if(NOT EXISTS "${MSVC_WINE_ROOT}/VC/Tools/MSVC")
    message(FATAL_ERROR
        "msvc-wine not found or incomplete at:\n"
        "  ${MSVC_WINE_ROOT}\n"
        "Install msvc-wine from: https://github.com/mstorsjo/msvc-wine"
    )
endif()

message(STATUS "Using msvc-wine at: ${MSVC_WINE_ROOT}")

# ------------------------------------------------------------
# Auto-detect MSVC and Windows SDK versions
# ------------------------------------------------------------
file(GLOB MSVC_VERSION_DIRS "${MSVC_WINE_ROOT}/VC/Tools/MSVC/*")
list(SORT MSVC_VERSION_DIRS)
list(REVERSE MSVC_VERSION_DIRS)
list(GET MSVC_VERSION_DIRS 0 LATEST_MSVC_DIR)
get_filename_component(MSVC_VERSION "${LATEST_MSVC_DIR}" NAME)

file(GLOB WINSDK_VERSION_DIRS "${MSVC_WINE_ROOT}/Windows Kits/10/Lib/*")
list(SORT WINSDK_VERSION_DIRS)
list(REVERSE WINSDK_VERSION_DIRS)
list(GET WINSDK_VERSION_DIRS 0 LATEST_WINSDK_DIR)
get_filename_component(WINSDK_VERSION "${LATEST_WINSDK_DIR}" NAME)

message(STATUS "Detected MSVC version: ${MSVC_VERSION}")
message(STATUS "Detected Windows SDK version: ${WINSDK_VERSION}")

# ------------------------------------------------------------
# Inject MSVC + Windows SDK paths
# ------------------------------------------------------------
set(MSVC_BASE "${MSVC_WINE_ROOT}/VC/Tools/MSVC/${MSVC_VERSION}")
set(WINSDK_BASE "${MSVC_WINE_ROOT}/Windows Kits/10")

# Library paths for linker
set(CMAKE_SYSTEM_LIBRARY_PATH
    "${MSVC_BASE}/lib/x64"
    "${WINSDK_BASE}/Lib/${WINSDK_VERSION}/um/x64"
    "${WINSDK_BASE}/Lib/${WINSDK_VERSION}/ucrt/x64"
)

# Include paths - these will be added as /imsvc flags
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    "${MSVC_BASE}/include"
    "${WINSDK_BASE}/Include/${WINSDK_VERSION}/ucrt"
    "${WINSDK_BASE}/Include/${WINSDK_VERSION}/um"
    "${WINSDK_BASE}/Include/${WINSDK_VERSION}/shared"
    "${WINSDK_BASE}/Include/${WINSDK_VERSION}/winrt"
    "${WINSDK_BASE}/Include/${WINSDK_VERSION}/cppwinrt"
)

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES})

# Also set for CMake's internal use
foreach(dir ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES})
    list(APPEND CMAKE_SYSTEM_INCLUDE_PATH "${dir}")
endforeach()

# Set LIB environment variable for linker (semicolon-separated on Windows)
string(REPLACE ";" ";" LIB_PATH_STR "${CMAKE_SYSTEM_LIBRARY_PATH}")
set(ENV{LIB} "${LIB_PATH_STR}")

# ------------------------------------------------------------
# Configure search paths
# ------------------------------------------------------------
set(CMAKE_FIND_ROOT_PATH "${MSVC_WINE_ROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# ------------------------------------------------------------
# Force remove /INCREMENTAL:YES from all linker flags
# ------------------------------------------------------------
foreach(flag_var
    CMAKE_EXE_LINKER_FLAGS CMAKE_EXE_LINKER_FLAGS_DEBUG CMAKE_EXE_LINKER_FLAGS_RELEASE
    CMAKE_EXE_LINKER_FLAGS_MINSIZEREL CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
    CMAKE_SHARED_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS_DEBUG CMAKE_SHARED_LINKER_FLAGS_RELEASE
    CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
    CMAKE_MODULE_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS_DEBUG CMAKE_MODULE_LINKER_FLAGS_RELEASE
    CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/INCREMENTAL:YES" "/INCREMENTAL:NO" ${flag_var} "${${flag_var}}")
    string(REGEX REPLACE "/INCREMENTAL ([^N][^O])" "/INCREMENTAL:NO" ${flag_var} "${${flag_var}}")
    set(${flag_var} "${${flag_var}}" CACHE STRING "" FORCE)
endforeach()

# ------------------------------------------------------------
# Compiler flags compatible with clang-cl
# ------------------------------------------------------------
# Add MSVC include paths explicitly
foreach(inc_dir ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES})
    add_compile_options(/imsvc"${inc_dir}")
endforeach()

add_compile_options(
    /std:c++23
    /Zc:__cplusplus
    /permissive-
    /EHsc
    /GR-
    /bigobj
    -Wno-invalid-offsetof
    -Wno-unused-command-line-argument
    -Wno-c++11-narrowing
    -Wno-reorder-ctor
    -Wno-missing-braces
    -Wno-unused-private-field
    -Wno-implicitly-unsigned-literal
)

# ------------------------------------------------------------
# Linker flags
# ------------------------------------------------------------
# Add library paths for linker (without quotes - CMake adds them)
foreach(lib_dir ${CMAKE_SYSTEM_LIBRARY_PATH})
    add_link_options(/LIBPATH:${lib_dir})
endforeach()

add_link_options(
    /INCREMENTAL:NO
    /NOLOGO
    /SUBSYSTEM:WINDOWS
    /MACHINE:X64
)

message(STATUS "clang-cl Windows DLL toolchain configured successfully.")