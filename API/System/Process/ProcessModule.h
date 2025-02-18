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

    // Module (�n�`�N���O���ǵ{���|���h�ӬۦP�W�٪� Exe�BDLL�A��p RPG Maker ���C��)�A�i��|�ݭn�ίS�O���������A���O�O����e�γ̦h��
    HMODULE GetModule(size_t PID, const wchar_t* name);

    DWORD_PTR GetModuleBaseAddress(size_t PID, LPCTSTR name);

    DWORD GetModuleSize(HANDLE hProcess, HMODULE hModule);

    DWORD GetModuleSizeByPidAndName(size_t PID, LPCTSTR name);

    void PrintAllModuleInfo(size_t PID);

    bool InMoudle(size_t PID,  DWORD_PTR Address);

    DWORD_PTR GetFunctionAddress(HANDLE hProcess, HMODULE hModule, const char* funcName);

private:
};