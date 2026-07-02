# CLAUDE.md

Guidance for Claude Code when working in this repository.

## Project Overview

**Notepad** is a minimal Notepad clone for Windows with a modern WinUI 3 interface, written in C++ (C++/WinRT) on the Windows App SDK. It is deployed unpackaged and self-contained: the build output folder carries the Windows App SDK runtime next to `Notepad.exe`, so users unzip and run without installing anything.

## Requirements

These decisions were made deliberately — do not change them without asking:

- **UI framework is WinUI 3** (Windows App SDK, C++/WinRT). The project was migrated from plain Win32 at the user's request; do not migrate back or to another framework without asking.
- **Deployment is unpackaged + self-contained** (`WindowsPackageType=None`, `WindowsAppSDKSelfContained=true`, `AppxPackage=false`). No MSIX packaging, no runtime installer requirement.
- **Feature scope is intentionally minimal**: New / Open / Save / Save As, plus Exit. Cut/copy/paste/undo come for free from the WinUI `TextBox` (Ctrl+X/C/V/Z) — no menu items for them. Unsaved-changes confirmation, word wrap toggle, and font selection are explicitly out of scope unless the user asks.
- **Modern design elements**: Mica system backdrop (`Window.SystemBackdrop`), WinUI `MenuBar`, `ContentDialog` for errors, automatic light/dark theme, per-monitor DPI (declared in `app.manifest`).
- **All UI strings and documentation are in English** (menus, window title, dialogs, README, this file).
- **Text encoding**: files are read/written as UTF-8 (no BOM), CRLF on save. The XAML `TextBox` reports line breaks as lone CR — `NormalizeToCrlf` handles this. Conversion uses `MultiByteToWideChar` / `WideCharToMultiByte`. No encoding detection.
- **File dialogs** use the Win32 common item dialogs (`IFileOpenDialog`/`IFileSaveDialog`) — synchronous and reliable for unpackaged apps; do not switch to `Windows.Storage.Pickers` without asking.
- **NuGet versions are pinned** in `src/packages.config` AND hard-coded in the import paths of `src/Notepad.vcxproj` (Microsoft.WindowsAppSDK, Microsoft.Windows.CppWinRT, Microsoft.Windows.SDK.BuildTools). When bumping a package, update both files consistently.

## Repository Layout

- `Notepad.sln` — solution (NuGet restores to `packages/` next to it)
- `src/Notepad.vcxproj` — project file (unpackaged, self-contained WinUI 3 desktop app)
- `src/App.xaml(.h/.cpp)` — application class, loads `XamlControlsResources`
- `src/MainWindow.xaml(.h/.cpp)` — the whole UI and file I/O logic
- `src/Project.idl` — WinRT class declarations (MainWindow)
- `src/pch.h/.cpp` — precompiled header
- `src/app.manifest` — DPI awareness declaration
- `src/Directory.Build.props`, `src/HybridCRT.props` — statically link the VC runtime/STL, dynamically link the UCRT
- `.github/workflows/ci.yml` — PR/main builds; `release.yml` — tag-triggered release builds

## Building

WinUI 3 / Windows App SDK **cannot** be cross-compiled with MinGW; MSVC on Windows is required. The development container is Linux, so local compilation is impossible — build verification happens on GitHub Actions (`windows-latest`).

```bash
nuget restore Notepad.sln
msbuild Notepad.sln /m /p:Configuration=Release /p:Platform=x64   # or ARM64
```

Output: `<Platform>/Release/Notepad/` at the repo root (exe + Windows App SDK runtime DLLs + resources.pri + MUI folders).

## Verification

- From the Linux container: push the branch and run the CI workflow (it has `workflow_dispatch`) via the GitHub API/MCP tools, then read the job logs. Both x64 and ARM64 must build.
- Functional testing happens on real Windows; Wine cannot run WinUI 3 apps reliably.
- Never commit `packages/`, `x64/`, `ARM64/`, or `Generated Files/` — they are gitignored.

## Releases (CI/CD)

- Every pull request (and push to `main`) triggers `.github/workflows/ci.yml`: MSVC x64 + ARM64 builds on `windows-latest`. `workflow_dispatch` is enabled for manual runs on any branch.
- Pushing a tag matching `v*` triggers `.github/workflows/release.yml`: builds both platforms, strips `.pdb`/`.lib`/`.exp`/`.winmd`, zips each output folder with README/LICENSE, and attaches both zips to a GitHub Release.
- The repository is public, so GitHub Actions minutes are free.

## Conventions

- C++/WinRT idioms throughout: `winrt::` projections, `com_ptr`, `hstring`. XAML event handlers are wired by name in XAML and implemented on the `implementation::MainWindow` type (no IDL entries needed for handlers).
- App/window state lives in `MainWindow` members (`m_currentFilePath`, `_hwnd`); title format is `<filename or "Untitled"> - Notepad`.
- The XAML-generated `wWinMain` is used (no hand-written entry point); `module.g.cpp` is compiled from `$(GeneratedFilesDir)`.
- New XAML pages/windows follow the sample pattern: `.xaml` + `.xaml.h` + `.xaml.cpp` + an entry in `Project.idl`, with `#if __has_include("X.g.cpp")` includes.
