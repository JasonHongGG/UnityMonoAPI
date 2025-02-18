#include "ProcessWindow.h"
#include "../../Utils/Utils.h"

HWND ProcessWindow::MainWindow = NULL;
std::string ProcessWindow::WindowName = "";
std::vector<ProcessWindow::EnumWindowsData> ProcessWindow::WindowList;
// ============================================================================================================================================================
//                                                                          ProcessWindow

BOOL ProcessWindow::isMainWindow(HWND handle)
{
    // 查看該 windows 是否有 "擁有者"(也就是他是子視窗)，以及該視窗是否可以顯示
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK ProcessWindow::EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    std::vector<EnumWindowsData>* windowList = reinterpret_cast<std::vector<EnumWindowsData>*>(lParam);

    DWORD windowProcessID;
    GetWindowThreadProcessId(hwnd, &windowProcessID);


    if (IsWindowVisible(hwnd))
    {
        wchar_t windowTitle[256];
        GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
        if (windowTitle[0] != L'\0' and windowTitle[0] != L'?' and windowTitle[0] != L' ' and windowTitle[0] != L'.' and wcscmp(windowTitle, L"MSCTFIME UI") != 0 and wcscmp(windowTitle, L"Default IME") != 0) {
            bool FindFlag = false;
            for (int i = 0; i < windowList->size(); i++) {
                if ((*windowList)[i].ProcessID == windowProcessID and _stricmp((*windowList)[i].WindowTitle.c_str(), Utils.UnicodeToUTF8(windowTitle).c_str()) == 0) {
                    (*windowList)[i].Windows.push_back(hwnd);
                    FindFlag = true;
                    break;
                }
            }
            if (!FindFlag) {
                EnumWindowsData Temp;
                Temp.ProcessID = windowProcessID;
                Temp.WindowTitle = Utils.UnicodeToUTF8(_wcsdup(windowTitle));
                Temp.Windows.push_back(hwnd);
                windowList->push_back(Temp);
            }
        }

    }

    return TRUE;
}

std::vector<ProcessWindow::EnumWindowsData> ProcessWindow::GetWindowList()
{
    std::vector<EnumWindowsData> windowList;
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&windowList));

    std::sort(windowList.begin() + 1, windowList.end());	// 排序 (以 struct 定義個 operator 行為排序)
    ProcessWindow::WindowList = windowList;
    return windowList;
}

void ProcessWindow::GetWindowsFromProcessID(DWORD dwProcessID, std::vector<HWND>& windows, const char* WindowTitle)
{
    std::vector<EnumWindowsData> windowList;
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&windowList));

    for (int i = 0; i < windowList.size(); i++) {
        //printf("[...] %X %s\n", windowList[i].ProcessID, windowList[i].WindowTitle.c_str());
        if (windowList[i].ProcessID == dwProcessID)
            if (strcmp(windowList[i].WindowTitle.c_str(), WindowTitle) == 0) {
                ProcessWindow::MainWindow = windowList[i].Windows[0];
                windows = std::move(windowList[i].Windows);
                break;
            }

    }
}

void ProcessWindow::GetWindowsNameFromHWND(HWND hwnd)
{
    wchar_t windowTitle[256];
    GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
    ProcessWindow::WindowName = Utils.UnicodeToUTF8(windowTitle);
}