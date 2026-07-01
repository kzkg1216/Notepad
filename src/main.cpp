// Minimal Win32 Notepad MVP: New / Open / Save / Save As.
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <string>
#include <vector>

namespace {

constexpr int ID_FILE_NEW = 1001;
constexpr int ID_FILE_OPEN = 1002;
constexpr int ID_FILE_SAVE = 1003;
constexpr int ID_FILE_SAVEAS = 1004;
constexpr int ID_FILE_EXIT = 1005;

constexpr wchar_t kWindowClassName[] = L"NotepadMVPWindowClass";
constexpr wchar_t kAppTitle[] = L"Notepad MVP";

HWND g_hEdit = nullptr;
std::wstring g_currentFilePath;

std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
    std::wstring wide(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), wide.data(), size);
    return wide;
}

std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
    std::string utf8(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), utf8.data(), size, nullptr, nullptr);
    return utf8;
}

void UpdateWindowTitle(HWND hwnd) {
    std::wstring title = g_currentFilePath.empty() ? L"Untitled" : g_currentFilePath;
    title += L" - ";
    title += kAppTitle;
    SetWindowTextW(hwnd, title.c_str());
}

bool OpenFileDialog(HWND hwnd, std::wstring& outPath) {
    wchar_t fileBuffer[MAX_PATH] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileNameW(&ofn)) return false;
    outPath = fileBuffer;
    return true;
}

bool SaveFileDialog(HWND hwnd, std::wstring& outPath) {
    wchar_t fileBuffer[MAX_PATH] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (!GetSaveFileNameW(&ofn)) return false;
    outPath = fileBuffer;
    return true;
}

void DoNew(HWND hwnd) {
    SetWindowTextW(g_hEdit, L"");
    g_currentFilePath.clear();
    UpdateWindowTitle(hwnd);
}

void DoOpen(HWND hwnd) {
    std::wstring path;
    if (!OpenFileDialog(hwnd, path)) return;

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
        MessageBoxW(hwnd, L"Failed to open the file.", kAppTitle, MB_OK | MB_ICONERROR);
        return;
    }
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    SetWindowTextW(g_hEdit, Utf8ToWide(contents).c_str());

    g_currentFilePath = path;
    UpdateWindowTitle(hwnd);
}

bool WriteEditContentsTo(HWND hwnd, const std::wstring& path) {
    int length = GetWindowTextLengthW(g_hEdit);
    std::wstring buffer(length, L'\0');
    if (length > 0) {
        GetWindowTextW(g_hEdit, buffer.data(), length + 1);
    }

    std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!file) {
        MessageBoxW(hwnd, L"Failed to save the file.", kAppTitle, MB_OK | MB_ICONERROR);
        return false;
    }
    std::string utf8 = WideToUtf8(buffer);
    file.write(utf8.data(), static_cast<std::streamsize>(utf8.size()));
    return true;
}

void DoSaveAs(HWND hwnd) {
    std::wstring path;
    if (!SaveFileDialog(hwnd, path)) return;
    if (!WriteEditContentsTo(hwnd, path)) return;

    g_currentFilePath = path;
    UpdateWindowTitle(hwnd);
}

void DoSave(HWND hwnd) {
    if (g_currentFilePath.empty()) {
        DoSaveAs(hwnd);
        return;
    }
    WriteEditContentsTo(hwnd, g_currentFilePath);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            g_hEdit = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL,
                0, 0, 0, 0, hwnd, nullptr,
                reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd, GWLP_HINSTANCE)), nullptr);
            UpdateWindowTitle(hwnd);
            return 0;
        }
        case WM_SIZE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            MoveWindow(g_hEdit, 0, 0, rect.right - rect.left, rect.bottom - rect.top, TRUE);
            return 0;
        }
        case WM_SETFOCUS:
            SetFocus(g_hEdit);
            return 0;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_FILE_NEW: DoNew(hwnd); return 0;
                case ID_FILE_OPEN: DoOpen(hwnd); return 0;
                case ID_FILE_SAVE: DoSave(hwnd); return 0;
                case ID_FILE_SAVEAS: DoSaveAs(hwnd); return 0;
                case ID_FILE_EXIT: DestroyWindow(hwnd); return 0;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HMENU BuildMenu() {
    HMENU menuBar = CreateMenu();
    HMENU fileMenu = CreatePopupMenu();
    AppendMenuW(fileMenu, MF_STRING, ID_FILE_NEW, L"New");
    AppendMenuW(fileMenu, MF_STRING, ID_FILE_OPEN, L"Open...");
    AppendMenuW(fileMenu, MF_STRING, ID_FILE_SAVE, L"Save");
    AppendMenuW(fileMenu, MF_STRING, ID_FILE_SAVEAS, L"Save As...");
    AppendMenuW(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(fileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");
    AppendMenuW(menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"File");
    return menuBar;
}

} // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kWindowClassName;
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, kWindowClassName, kAppTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) return 0;

    SetMenu(hwnd, BuildMenu());
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
