
#include "ProcessInfo.h"
#include "../../Utils/Utils.h"

size_t ProcessInfo::PID = 0;
std::string ProcessInfo::ProcessName = "";
HANDLE  ProcessInfo::hProcess = NULL;
size_t ProcessInfo::ProcIs64Bit = true;
size_t ProcessInfo::ProcOffestSub = 0;
size_t ProcessInfo::ProcOffestAdd = 8;
// ============================================================================================================================================================
//                                                                          ProcessInfo
    // Process
HANDLE ProcessInfo::GetProcessHandle(const wchar_t* name)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return NULL;
    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(processEntry);

    if (Process32FirstW(hSnapshot, &processEntry)) {
        do {
            if (!_wcsicmp(processEntry.szExeFile, name)) {
                PID = processEntry.th32ProcessID;   // PID
                CloseHandle(hSnapshot);
                ProcessInfo::hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)PID); // Handler
            }
        } while (Process32NextW(hSnapshot, &processEntry));
    }

    return ProcessInfo::hProcess;
};

size_t ProcessInfo::GetPID(const wchar_t* name)
{
    if (name != NULL)
    {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Process
        if (hSnapshot == INVALID_HANDLE_VALUE) return NULL;
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32FirstW(hSnapshot, &processEntry)) {
            do {
                //wprintf(L"Process name: %s\n", processEntry.szExeFile);
                if (!_wcsicmp(processEntry.szExeFile, name)) {
                    ProcessInfo::PID = processEntry.th32ProcessID;
                    CloseHandle(hSnapshot);
                    ProcessInfo::hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)ProcessInfo::PID);
                }
            } while (Process32NextW(hSnapshot, &processEntry));
        }
    }
    return ProcessInfo::PID;
}

void ProcessInfo::GetProcessNameByPID(size_t PID)
{
    std::wstring ProcessName = L"";
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Process
    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(processEntry);

    while (Process32NextW(hSnapshot, &processEntry)) {
        if (processEntry.th32ProcessID == (DWORD)PID) {
            ProcessName = processEntry.szExeFile;
            CloseHandle(hSnapshot);
            break;
        }
    }
    ProcessInfo::ProcessName = Utils.UnicodeToUTF8(ProcessName.c_str());
}

bool ProcessInfo::ProcessIs64Bit()
{

    BOOL isWin64 = FALSE;
    BOOL isWow64 = FALSE;

    if (!IsWow64Process(ProcessInfo::hProcess, &isWow64)) {
        std::cerr << "Failed to Determine If Process Architecture is 64Bit. Error code: " << GetLastError() << std::endl;
        CloseHandle(ProcessInfo::hProcess);
        return false;
    }

    if (isWow64) isWin64 = FALSE;
    else {
        SYSTEM_INFO systemInfo = { 0 };
        GetNativeSystemInfo(&systemInfo);  //System Info
        if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
            isWin64 = TRUE;
        else if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
            isWin64 = FALSE;
        else
            return FALSE;
    }

    if (!isWin64) {
        ProcessInfo::ProcOffestSub = 4;       // For 32Bit Application
        ProcessInfo::ProcOffestAdd = 4;
    }

    ProcessInfo::ProcIs64Bit = isWin64;
    return isWin64;
}

