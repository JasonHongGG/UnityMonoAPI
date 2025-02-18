#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>
#include <psapi.h>

class ProcessModule
{
public:
    static HMODULE hMainMoudle;

    // Module (要注意的是有些程式會有多個相同名稱的 Exe、DLL，比如 RPG Maker 的遊戲)，可能會需要用特別的條件選取，像是記憶體占用最多的
    HMODULE GetModule(size_t PID, const wchar_t* name);

    DWORD_PTR GetModuleBaseAddress(size_t PID, LPCTSTR name);

    DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule);

    DWORD GetModuleSizeByPidAndName(size_t PID, LPCTSTR name);

    void PrintAllModuleInfo(size_t PID);

    bool InMoudle(size_t PID,  DWORD_PTR Address);

    DWORD_PTR GetFunctionAddress(HANDLE hProcess, HMODULE hModule, const char* funcName);

private:
};