# Notepad

A minimal, lightweight Notepad clone for Windows, written in C++ with the plain Win32 API (no external GUI libraries). It ships as a single, statically linked executable with no runtime dependencies.

## Features (MVP)

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

The build commands above use plain CLI invocations only, so they can be dropped into a CI pipeline (e.g. GitHub Actions) as-is. A CI workflow is not included yet but is planned.
