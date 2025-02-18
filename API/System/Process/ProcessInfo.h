#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <TlHelp32.h>
#include <tchar.h>
#include <psapi.h>


class ProcessInfo
{
public:
    // Parameter
    static size_t PID;
    static std::string ProcessName;
    static HANDLE hProcess;
    static size_t ProcIs64Bit;
    static size_t ProcOffestSub;
    static size_t ProcOffestAdd;

    HANDLE GetProcessHandle(const wchar_t* name);

    size_t GetPID(const wchar_t* name = NULL);

    void GetProcessNameByPID(size_t PID);

    bool ProcessIs64Bit();

private:
};
