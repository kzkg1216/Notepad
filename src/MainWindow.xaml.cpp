#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "microsoft.ui.xaml.window.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Microsoft::UI::Xaml;
}

namespace
{

constexpr wchar_t kAppTitle[] = L"Notepad";

std::wstring Utf8ToWide(const std::string& utf8)
{
    if (utf8.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
    std::wstring wide(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), wide.data(), size);
    return wide;
}

std::string WideToUtf8(const std::wstring& wide)
{
    if (wide.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
    std::string utf8(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), utf8.data(), size, nullptr, nullptr);
    return utf8;
}

// The XAML TextBox reports line breaks as a lone CR; normalize everything to CRLF on disk.
std::wstring NormalizeToCrlf(const std::wstring& text)
{
    std::wstring out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i)
    {
        wchar_t c = text[i];
        if (c == L'\r')
        {
            out += L"\r\n";
            if (i + 1 < text.size() && text[i + 1] == L'\n') ++i;
        }
        else if (c == L'\n')
        {
            out += L"\r\n";
        }
        else
        {
            out += c;
        }
    }
    return out;
}

} // namespace

namespace winrt::Notepad::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        UpdateTitle();
        SetWindowSize(GetWindowHandle(), 900, 640);
    }

    HWND MainWindow::GetWindowHandle()
    {
        if (_hwnd == nullptr)
        {
            Window window = *this;
            window.as<IWindowNative>()->get_WindowHandle(&_hwnd);
        }
        return _hwnd;
    }

    void MainWindow::SetWindowSize(HWND hwnd, int width, int height)
    {
        // Win32 uses pixels and WinUI 3 uses effective pixels, so apply the DPI scale factor.
        const UINT dpi = GetDpiForWindow(hwnd);
        const float scalingFactor = static_cast<float>(dpi) / 96;
        const int widthScaled = static_cast<int>(width * scalingFactor);
        const int heightScaled = static_cast<int>(height * scalingFactor);

        SetWindowPos(hwnd, nullptr, 0, 0, widthScaled, heightScaled, SWP_NOMOVE | SWP_NOZORDER);
    }

    void MainWindow::UpdateTitle()
    {
        std::wstring title = m_currentFilePath.empty() ? L"Untitled" : m_currentFilePath;
        title += L" - ";
        title += kAppTitle;
        Title(winrt::hstring{ title });
    }

    void MainWindow::ShowError(winrt::hstring const& message)
    {
        Microsoft::UI::Xaml::Controls::ContentDialog dialog;
        dialog.XamlRoot(Content().XamlRoot());
        dialog.Title(winrt::box_value(winrt::hstring{ kAppTitle }));
        dialog.Content(winrt::box_value(message));
        dialog.CloseButtonText(L"OK");
        dialog.ShowAsync();
    }

    bool MainWindow::PickFile(bool forSave, std::wstring& outPath)
    {
        winrt::com_ptr<IFileDialog> dialog;
        HRESULT hr = CoCreateInstance(
            forSave ? __uuidof(FileSaveDialog) : __uuidof(FileOpenDialog),
            nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(dialog.put()));
        if (FAILED(hr)) return false;

        static constexpr COMDLG_FILTERSPEC filters[] = {
            { L"Text Files (*.txt)", L"*.txt" },
            { L"All Files (*.*)", L"*.*" },
        };
        dialog->SetFileTypes(ARRAYSIZE(filters), filters);
        dialog->SetDefaultExtension(L"txt");

        if (dialog->Show(GetWindowHandle()) != S_OK) return false;

        winrt::com_ptr<IShellItem> item;
        if (FAILED(dialog->GetResult(item.put()))) return false;

        wchar_t* rawPath = nullptr;
        if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath))) return false;
        outPath = rawPath;
        CoTaskMemFree(rawPath);
        return true;
    }

    bool MainWindow::SaveEditorTo(std::wstring const& path)
    {
        std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
        if (!file)
        {
            ShowError(L"Failed to save the file.");
            return false;
        }
        std::string utf8 = WideToUtf8(NormalizeToCrlf(std::wstring{ Editor().Text() }));
        file.write(utf8.data(), static_cast<std::streamsize>(utf8.size()));
        return true;
    }

    void MainWindow::OnNewClick(winrt::IInspectable const&, winrt::RoutedEventArgs const&)
    {
        Editor().Text(L"");
        m_currentFilePath.clear();
        UpdateTitle();
    }

    void MainWindow::OnOpenClick(winrt::IInspectable const&, winrt::RoutedEventArgs const&)
    {
        std::wstring path;
        if (!PickFile(false, path)) return;

        std::ifstream file(path.c_str(), std::ios::binary);
        if (!file)
        {
            ShowError(L"Failed to open the file.");
            return;
        }
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Editor().Text(Utf8ToWide(contents));

        m_currentFilePath = path;
        UpdateTitle();
    }

    void MainWindow::OnSaveClick(winrt::IInspectable const& sender, winrt::RoutedEventArgs const& e)
    {
        if (m_currentFilePath.empty())
        {
            OnSaveAsClick(sender, e);
            return;
        }
        SaveEditorTo(m_currentFilePath);
    }

    void MainWindow::OnSaveAsClick(winrt::IInspectable const&, winrt::RoutedEventArgs const&)
    {
        std::wstring path;
        if (!PickFile(true, path)) return;
        if (!SaveEditorTo(path)) return;

        m_currentFilePath = path;
        UpdateTitle();
    }

    void MainWindow::OnExitClick(winrt::IInspectable const&, winrt::RoutedEventArgs const&)
    {
        Close();
    }
}
