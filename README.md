# Payday3-Internal
Payday 3 Internal Cheat

## Build Requirements

### Windows
- **Visual Studio 2022** or newer (with C++ Desktop Development workload)
- **CMake 3.20+**
- **MSVC** or **Clang (clang-cl)** compiler

### Proton-GE Support
This DLL is compatible with **Proton-GE** on Linux. Build the DLL on Windows, then inject it using tools like:
- Process Hacker (via Proton)
- Other DLL injection tools compatible with Wine/Proton

## Building

### Using Visual Studio
```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

### Using Ninja (faster builds)
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Using MSVC Toolchain (explicit)
```bash
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/msvc-windows.cmake
cmake --build build
```

## Output
The compiled DLL will be located at: `build/bin/Payday3Internal.dll`

## Notes
- This project **only supports Windows builds** (MSVC/Clang)
- Native Linux builds are **not supported**
- For Linux users: Use **Proton-GE** to run Payday 3 and inject the Windows DLL
