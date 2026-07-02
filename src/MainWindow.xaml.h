#pragma once
#include "MainWindow.g.h"
#include "pch.h"

namespace winrt::Notepad::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void OnNewClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnOpenClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnSaveClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnSaveAsClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnExitClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        HWND GetWindowHandle();
        void SetWindowSize(HWND hwnd, int width, int height);
        void UpdateTitle();
        bool PickFile(bool forSave, std::wstring& outPath);
        bool SaveEditorTo(std::wstring const& path);
        void ShowError(winrt::hstring const& message);

        HWND _hwnd{ nullptr };
        std::wstring m_currentFilePath;
    };
}

namespace winrt::Notepad::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
