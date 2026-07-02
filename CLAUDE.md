# CLAUDE.md

Guidance for Claude Code when working in this repository.

## Project Overview

**Notepad** is a minimal, lightweight Notepad clone for Windows, written in C++ using the plain Win32 API. It ships as a single, statically linked `notepad.exe` with no runtime dependencies.

## Requirements

These decisions were made deliberately — do not change them without asking:

- **Lightweight above all**: no external GUI libraries (no Qt, no Dear ImGui, no Electron-style runtimes). UI is built directly on the Win32 API with the standard multiline EDIT control.
- **Feature scope is intentionally minimal**: New / Open / Save / Save As, plus Exit. Cut/copy/paste/undo come for free from the EDIT control's built-in shortcuts (Ctrl+X/C/V/Z) — no menu items for them. Unsaved-changes confirmation, word wrap toggle, and font selection are explicitly out of scope unless the user asks.
- **All UI strings and documentation are in English** (menus, window title, dialogs, README, this file).
- **Single source file**: the whole app lives in `src/main.cpp`. Do not split it into modules while the scope stays this small.
- **Dependency-free binary**: MinGW builds statically link libgcc/libstdc++; MSVC builds statically link the CRT (`MSVC_RUNTIME_LIBRARY` in CMakeLists.txt). Keep it that way.
- **Text encoding**: files are read/written as UTF-8 (no BOM). The EDIT control works in UTF-16; conversion is done with `MultiByteToWideChar` / `WideCharToMultiByte`. No encoding detection.

## Repository Layout

- `src/main.cpp` — entire application (window, menu, file I/O)
- `CMakeLists.txt` — build definition (CMake >= 3.15)
- `.github/workflows/release.yml` — tag-triggered release builds
- `README.md` — user-facing docs and build instructions

## Building

Development happens in a Linux container; the app targets Windows, so builds are cross-compiled.

### Linux → Windows x86_64 (MinGW-w64)

```bash
sudo apt-get install -y mingw-w64
cmake -B build -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
cmake --build build
# produces build/notepad.exe
```

### Windows (MSVC)

```bash
cmake -B build            # or: cmake -B build -A ARM64
cmake --build build --config Release
```

Note: apt's mingw-w64 has **no aarch64 target**, so Windows ARM64 builds require MSVC (done in CI on `windows-latest`).

## Verification

- The `.exe` cannot run natively on Linux; verification is (a) a clean MinGW cross-compile and (b) code review of Win32 API usage. Use Wine for a smoke test if installed; otherwise final functional testing happens on real Windows.
- Delete the `build/` directory before committing — build artifacts are gitignored but never belong in a commit.

## Releases (CI/CD)

- Pushing a tag matching `v*` triggers `.github/workflows/release.yml`: a matrix build on `windows-latest` compiles x86_64 (`cmake -A x64`) and ARM64 (`cmake -A ARM64`) with MSVC, zips each with README/LICENSE, and attaches both to a GitHub Release.
- The repository is public, so GitHub Actions minutes are free.
- Keep build steps as plain CLI commands so README instructions and CI stay interchangeable.

## Conventions

- Wide-character (`W`-suffixed) Win32 APIs everywhere; `UNICODE`/`_UNICODE` are defined at the top of `main.cpp`.
- Entry point is `WinMain` (not `wWinMain`) — MinGW's CRT requires it.
- `std::ifstream`/`std::ofstream` need `path.c_str()` (a `const wchar_t*`) on MinGW; passing `std::wstring` directly does not compile.
- App state is a few globals (`g_hEdit`, `g_currentFilePath`); title format is `<filename or "Untitled"> - Notepad`.
