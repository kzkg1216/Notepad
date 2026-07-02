# Notepad

A minimal, lightweight Notepad clone for Windows, written in C++ with the plain Win32 API (no external GUI libraries). It ships as a single, statically linked executable with no runtime dependencies.

## Features

- New
- Open
- Save
- Save As

Basic text editing (cut/copy/paste/undo) is provided for free by the standard Win32 EDIT control via its built-in keyboard shortcuts (Ctrl+X/C/V/Z).

## Building

### Cross-compiling from Linux (MinGW-w64)

```bash
sudo apt-get install -y mingw-w64
cmake -B build -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
cmake --build build
# produces build/notepad.exe
```

### Native build on Windows (MinGW or MSVC)

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

or, with Visual Studio's generator:

```bash
cmake -B build
cmake --build build --config Release
```

To target Windows on ARM64 with MSVC, pass `-A ARM64` (requires the ARM64 build tools component):

```bash
cmake -B build -A ARM64
cmake --build build --config Release
```

## Releases

Pushing a tag matching `v*` (e.g. `v0.1.0`) triggers the `Release` GitHub Actions workflow (`.github/workflows/release.yml`), which builds `notepad.exe` for both **Windows x86_64** and **Windows ARM64** using MSVC on `windows-latest` runners, and publishes each as a separate zip attached to a GitHub Release.

```bash
git tag v0.1.0
git push origin v0.1.0
```
