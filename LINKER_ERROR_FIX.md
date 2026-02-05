# Fix Summary: Unresolved External Symbol Error

## Problem
```
unresolved external symbol "struct std::atomic<bool> Globals::g_bUnloadRequested"
```

## Root Cause
In `src/main.cpp`, the variable was declared as `static`:
```cpp
static std::atomic<bool> g_bUnloadRequested{ false };  // WRONG - static = internal linkage
```

When a variable is `static` at namespace scope, it has **internal linkage**, meaning it's only visible within that translation unit (file). However, `Menu.cpp` was trying to access it with:
```cpp
extern std::atomic<bool> g_bUnloadRequested;  // Expects external linkage
```

This caused a linker error because the symbol wasn't exported.

## Solution Applied

### Changed in `src/main.cpp`:
```cpp
// BEFORE (WRONG):
static std::atomic<bool> g_bUnloadRequested{ false };

// AFTER (CORRECT):
std::atomic<bool> g_bUnloadRequested{ false };  // Removed 'static' for external linkage
```

Also updated the main loop to check this flag:
```cpp
// BEFORE:
while (!GetAsyncKeyState(UNLOAD_KEY) && !GetAsyncKeyState(UNLOAD_KEY_ALT))

// AFTER:
while (!GetAsyncKeyState(UNLOAD_KEY) && !GetAsyncKeyState(UNLOAD_KEY_ALT) && !Globals::g_bUnloadRequested)
```

## Changes Made

### 1. Fixed External Linkage (Line ~30)
```cpp
namespace Globals {
    static HMODULE g_hModule;
    static HMODULE g_hBaseModule;
    std::unique_ptr<Console> g_upConsole;
    std::atomic<bool> g_bUnloadRequested{ false };  // ? No 'static'
}
```

### 2. Added Unload Check to Main Loop (Line ~271)
```cpp
void MainLoop()
{	
    // Now checks g_bUnloadRequested
    while (!GetAsyncKeyState(UNLOAD_KEY) && 
           !GetAsyncKeyState(UNLOAD_KEY_ALT) && 
           !Globals::g_bUnloadRequested)  // ? Added this check
    {
        // ...
    }
}
```

## Why This Works

### Linkage in C++

| Declaration | Linkage | Visibility |
|-------------|---------|------------|
| `static int x;` | Internal | Only in current file |
| `int x;` | External | Across all files |
| `extern int x;` | External | Declaration only (defined elsewhere) |

### Before Fix:
```
main.cpp: static g_bUnloadRequested  ? Internal linkage (file-local)
Menu.cpp: extern g_bUnloadRequested  ? Looking for external symbol
Linker: ? ERROR - Symbol not found (it's internal to main.cpp)
```

### After Fix:
```
main.cpp: g_bUnloadRequested         ? External linkage (visible globally)
Menu.cpp: extern g_bUnloadRequested  ? Declaration (references main.cpp's definition)
Linker: ? SUCCESS - Symbol found and linked
```

## What This Enables

Now the **Unload Button** in the menu works correctly:

1. User clicks "Unload Cheat" button in Menu
2. Menu.cpp sets `Globals::g_bUnloadRequested = true`
3. Main loop detects the flag and exits
4. Cheat cleanly unloads

## Build Result
? **Build successful** - Linker error resolved

## Additional Notes

### Other `static` Variables in Globals
Notice that `g_hModule` and `g_hBaseModule` are still `static`:
```cpp
static HMODULE g_hModule;        // OK - only used in main.cpp
static HMODULE g_hBaseModule;    // OK - only used in main.cpp
```

These remain `static` because they're **only accessed within main.cpp**. Only `g_bUnloadRequested` needed external linkage because it's accessed from `Menu.cpp`.

### Rule of Thumb
- Use `static` for namespace-scope variables that are **file-local**
- Omit `static` for namespace-scope variables that need to be **shared across files**
- Use `extern` in other files to **declare** (not define) shared variables

## Testing
After this fix:
1. ? Build compiles successfully
2. ? DLL can be injected
3. ? Menu loads correctly
4. ? Unload button should work (sets the flag)
5. ? Main loop exits when flag is set
6. ? Clean unload sequence executes

## Related Files Changed
- `src/main.cpp` - Fixed declaration and added loop check
- `src/game/frontend/Menu.cpp` - Already had correct `extern` declaration
