#!/bin/bash
# ============================================================
# SDK patch script for Linux compilation
# Minimal fixes for case sensitivity and clang strictness
# ============================================================

set -e

SDK_DIR="src/types/gameoffsets/SDK"
UNREAL_CONTAINERS="src/types/gameoffsets/UnrealContainers.hpp"

echo "Applying minimal patches for Linux compatibility..."

# Fix 1: Create case-sensitive symlink for BP_Keypad
ln -sf BP_KeyPad_classes.hpp "${SDK_DIR}/BP_Keypad_classes.hpp"

# Fix 2: Fix static_cast to reinterpret_cast for incomplete types
sed -i 's/static_cast<UClass\*>/reinterpret_cast<UClass*>/g' "${SDK_DIR}/Basic.hpp"

# Fix 3: Comment out unused operator-- that fails in clang
sed -i '/inline TContainerIterator& operator--()/,/}/s/^/\/\/ /' "${UNREAL_CONTAINERS}"

# Fix 4: Comment out problematic struct fields
sed -i 's/^\(\s*struct InvalidUseOfTDelegate\s.*TemplateParamIsNotAFunctionSignature;.*\)/\/\/ \1/' "${SDK_DIR}/Basic.hpp"
sed -i 's/^\(\s*struct InvalidUseOfTMulticastInlineDelegate\s.*TemplateParamIsNotAFunctionSignature;.*\)/\/\/ \1/' "${SDK_DIR}/Basic.hpp"

# Fix 5: Replace member access on incomplete type with offset-based access
# ClassDefaultObject is at offset 0xB0 in UClass on x64
sed -i 's/StaticClass->ClassDefaultObject/*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(StaticClass) + 0xB0)/g' "${SDK_DIR}/Basic.hpp"

echo "Patches applied successfully!"
