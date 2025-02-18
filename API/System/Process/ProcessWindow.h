#pragma once

#pragma once

#include <Windows.h>
#include <string>
#include <vector>


class ProcessWindow
{
public:
    struct EnumWindowsData {
        std::vector<HWND> Windows;
        DWORD ProcessID = 0x0;
        std::string WindowTitle = "";

        // 排序會以這個為準
        bool operator<(const EnumWindowsData& other) const {
            return WindowTitle < other.WindowTitle;
        }
    };
    static HWND MainWindow;
    static std::string WindowName;
    static std::vector<EnumWindowsData> WindowList;
    int UserWindowWidth = GetSystemMetrics(SM_CXSCREEN);
    int UserWindowHeight = GetSystemMetrics(SM_CYSCREEN);

    BOOL isMainWindow(HWND handle);

    std::vector<EnumWindowsData> GetWindowList();

    void GetWindowsFromProcessID(DWORD dwProcessID, std::vector<HWND>& windows, const char* WindowTitle);

    void GetWindowsNameFromHWND(HWND hwnd);

private:
    static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
};