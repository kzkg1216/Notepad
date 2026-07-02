# Notepad

A minimal Notepad clone for Windows with a modern WinUI 3 interface, written in C++ (C++/WinRT) on the Windows App SDK. The app is deployed **unpackaged and self-contained**: the build output folder carries the Windows App SDK runtime alongside `Notepad.exe`, so nothing needs to be installed first — unzip and run.

## Features

- New / Open / Save / Save As (File menu, with Ctrl+N / Ctrl+O / Ctrl+S / Ctrl+Shift+S accelerators)
- Modern Windows 11 look: Mica window backdrop, WinUI 3 menu bar and dialogs, automatic light/dark theme, per-monitor DPI awareness
- Basic text editing (cut/copy/paste/undo) comes from the WinUI `TextBox` via its built-in shortcuts (Ctrl+X/C/V/Z)
- Files are read and written as UTF-8 (no BOM), CRLF line endings on save

## Requirements

- Windows 10 21H2 (build 22000) or later; Windows 11 recommended (Mica backdrop falls back to a solid color on Windows 10)

## Building

Building requires Windows with Visual Studio 2022 (Desktop development with C++ workload, plus the ARM64 build tools component for ARM64 targets). WinUI 3 / Windows App SDK cannot be cross-compiled with MinGW.

```bash
nuget restore Notepad.sln
msbuild Notepad.sln /m /p:Configuration=Release /p:Platform=x64
```

The self-contained output lands in `x64/Release/` (or `ARM64/Release/` when built with `/p:Platform=ARM64`).

You can also open `Notepad.sln` in Visual Studio 2022 and build from the IDE.

## Releases

Pushing a tag matching `v*` (e.g. `v0.1.0`) triggers the `Release` GitHub Actions workflow (`.github/workflows/release.yml`), which builds the app for both **Windows x86_64** and **Windows ARM64** using MSVC on `windows-latest` runners, and publishes each self-contained build as a separate zip attached to a GitHub Release.

```bash
git tag v0.1.0
git push origin v0.1.0
```
