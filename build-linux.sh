#!/bin/bash
# ============================================================
# Build script for cross-compiling Windows DLL on Linux
# ============================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Payday3Internal - Linux Build Script${NC}"
echo -e "${GREEN}Target: Windows DLL${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Check for required tools
echo -e "${YELLOW}Checking dependencies...${NC}"

command -v cmake >/dev/null 2>&1 || { echo -e "${RED}Error: cmake not found. Install it first.${NC}"; exit 1; }
command -v clang-cl >/dev/null 2>&1 || { echo -e "${RED}Error: clang-cl not found. Install LLVM with clang-cl support.${NC}"; exit 1; }
command -v lld-link >/dev/null 2>&1 || { echo -e "${RED}Error: lld-link not found. Install LLVM with lld support.${NC}"; exit 1; }

echo -e "${GREEN}✓ CMake found: $(cmake --version | head -n1)${NC}"
echo -e "${GREEN}✓ clang-cl found: $(clang-cl --version | head -n1)${NC}"
echo -e "${GREEN}✓ lld-link found${NC}"
echo ""

# Check for msvc-wine
MSVC_WINE_PATH="${HOME}/my_msvc/opt/msvc"
if [ ! -d "$MSVC_WINE_PATH/VC/Tools/MSVC" ]; then
    echo -e "${RED}Error: msvc-wine not found at $MSVC_WINE_PATH${NC}"
    echo -e "${YELLOW}Install msvc-wine from: https://github.com/mstorsjo/msvc-wine${NC}"
    echo ""
    echo "Quick install instructions:"
    echo "1. git clone https://github.com/mstorsjo/msvc-wine.git"
    echo "2. cd msvc-wine"
    echo "3. ./vsdownload.py --accept-license --dest \$HOME/my_msvc"
    echo "4. ./install.sh \$HOME/my_msvc"
    exit 1
fi
echo -e "${GREEN}✓ msvc-wine found at $MSVC_WINE_PATH${NC}"
echo ""

# Build directory
BUILD_DIR="build-linux"
echo -e "${YELLOW}Configuring build in ${BUILD_DIR}...${NC}"

# Clean build directory if requested
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Apply SDK patches for Linux compatibility
echo -e "${YELLOW}Applying SDK patches for Linux...${NC}"
./patch-sdk-for-linux.sh

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${YELLOW}Running CMake configuration...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/linux.cmake \
    -G "Ninja" 2>/dev/null || cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/linux.cmake \
    -G "Unix Makefiles"

# Create minhook case-sensitive symlink
echo -e "${YELLOW}Creating MinHook symlink...${NC}"
if [ -f "_deps/minhook-src/include/MinHook.h" ]; then
    ln -sf MinHook.h _deps/minhook-src/include/minhook.h
fi

echo ""
echo -e "${YELLOW}Building DLL...${NC}"
cmake --build . -j$(nproc)

echo ""
if [ -f "bin/Payday3Internal.dll" ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}Build successful!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo -e "${GREEN}Output: $(pwd)/bin/Payday3Internal.dll${NC}"
    echo ""
    ls -lh bin/Payday3Internal.dll
    echo ""
    echo -e "${YELLOW}File type:${NC}"
    file bin/Payday3Internal.dll
    echo ""
    echo -e "${GREEN}You can now inject this DLL using Process Hacker under Proton-GE${NC}"
else
    echo -e "${RED}Build failed - DLL not found!${NC}"
    exit 1
fi
