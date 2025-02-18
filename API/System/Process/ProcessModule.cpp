#include "ProcessModule.h"
#include "ProcessInfo.h" //<= GetModule Need
#include "../../Utils/Utils.h" //<= GetModule Need

HMODULE ProcessModule::hMainMoudle = 0;

// Module (要注意的是有些程式會有多個相同名稱的 Exe、DLL，比如 RPG Maker 的遊戲)，可能會需要用特別的條件選取，像是記憶體占用最多的
HMODULE ProcessModule::GetModule(size_t PID, const wchar_t* name)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32W moduleEntry;             // szModule 和 szExePath 是 Unicode 
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (!_wcsicmp(moduleEntry.szModule, name)) {
                CloseHandle(hSnapshot);
                if(Utils.UTF8ToUnicode(ProcessInfo::ProcessName.c_str()).find(name) != std::wstring::npos)
                    ProcessModule::hMainMoudle = moduleEntry.hModule;
                return moduleEntry.hModule;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
    return 0;
}

DWORD_PTR ProcessModule::GetModuleBaseAddress(size_t PID, LPCTSTR name)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32 moduleEntry32;            // MODULEENTRY32 就是 MODULEENTRY32W 根本在搞，都是 Unicode
    moduleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &moduleEntry32)) {
        do {
            //_tprintf(TEXT("\n\n MODULE NAME: %s"), moduleEntry32.szModule);     // 用來 print Unicode 的
            if (_tcsicmp(moduleEntry32.szModule, name) == 0) {
                CloseHandle(hSnapshot);
                return (DWORD_PTR)moduleEntry32.modBaseAddr;
            }
        } while (Module32Next(hSnapshot, &moduleEntry32));
    };

    CloseHandle(hSnapshot);
    return 0;
}

DWORD ProcessModule::GetModuleSize(HANDLE hProcess, HMODULE hModule)
{
    MODULEINFO moduleInfo;
    if (GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo)))
        return moduleInfo.SizeOfImage;
    return 0;
}

DWORD ProcessModule::GetModuleSizeByPidAndName(size_t PID, LPCTSTR name) {
    DWORD dwSize = 0;

    do {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
        if (hSnapshot == INVALID_HANDLE_VALUE) { continue; }

        MODULEENTRY32 ModuleEntry32;
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &ModuleEntry32)) {
            do {
                if (_tcsicmp(ModuleEntry32.szModule, name) == 0) {
                    dwSize = ModuleEntry32.modBaseSize;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }

        CloseHandle(hSnapshot);
    } while (!dwSize);

    return dwSize;
}

void ProcessModule::PrintAllModuleInfo(size_t PID)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32W moduleEntry;             // szModule 和 szExePath 是 Unicode 
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {

            printf("[ Module Name ] %ls\n", moduleEntry.szModule);
            printf("[ Module Base Address ] %p \t [Moduel Size] %5ld\n", (void*)(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);


        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
}

bool ProcessModule::InMoudle(size_t PID, DWORD_PTR Address)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)PID);
    MODULEENTRY32W moduleEntry;             // szModule 和 szExePath 是 Unicode 
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (Address >= reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr) && Address <= (reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr) + moduleEntry.modBaseSize)) {
                //printf("[ Find !!!! ] \n");
                //printf("[ Module Name ] %ls\n", moduleEntry.szModule);
                //printf("[ Module Base Address ] %p \t [Moduel Size] %5d\n", reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);

                CloseHandle(hSnapshot);
                return true;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
    return false;
}

DWORD_PTR ProcessModule::GetFunctionAddress(HANDLE hProcess, HMODULE hModule, const char* funcName) {
    BYTE peHeader[0x400];  // 用於存儲 PE Header
    SIZE_T bytesRead;

    // 讀取 PE Header
    if (!ReadProcessMemory(hProcess, hModule, peHeader, sizeof(peHeader), &bytesRead)) {
        std::cerr << "無法讀取 PE Header!" << std::endl;
        return 0;
    }

    // 取得 PE 結構
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)peHeader;
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(peHeader + dosHeader->e_lfanew);
    IMAGE_OPTIONAL_HEADER* optionalHeader = &ntHeaders->OptionalHeader;
    IMAGE_DATA_DIRECTORY* exportDirectoryData = &optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    // 計算 Export Table 在記憶體內的地址
    DWORD exportTableRVA = exportDirectoryData->VirtualAddress;
    IMAGE_EXPORT_DIRECTORY exportTable;

    if (!ReadProcessMemory(hProcess, (BYTE*)hModule + exportTableRVA, &exportTable, sizeof(exportTable), &bytesRead)) {
        std::cerr << "無法讀取 Export Table!" << std::endl;
        return 0;
    }

    // 讀取函數名稱和地址表
    DWORD* nameTableRVA = new DWORD[exportTable.NumberOfNames];
    DWORD* funcAddrTableRVA = new DWORD[exportTable.NumberOfFunctions];
    WORD* nameOrdinalTableRVA = new WORD[exportTable.NumberOfNames];

    ReadProcessMemory(hProcess, (BYTE*)hModule + exportTable.AddressOfNames, nameTableRVA, exportTable.NumberOfNames * sizeof(DWORD), &bytesRead);
    ReadProcessMemory(hProcess, (BYTE*)hModule + exportTable.AddressOfFunctions, funcAddrTableRVA, exportTable.NumberOfFunctions * sizeof(DWORD), &bytesRead);
    ReadProcessMemory(hProcess, (BYTE*)hModule + exportTable.AddressOfNameOrdinals, nameOrdinalTableRVA, exportTable.NumberOfNames * sizeof(WORD), &bytesRead);

    // 遍歷函數名稱，查找 `getMyFunction`
    DWORD functionRVA = 0;
    for (DWORD i = 0; i < exportTable.NumberOfNames; i++) {
        char funcNameBuffer[256];

        ReadProcessMemory(hProcess, (BYTE*)hModule + nameTableRVA[i], funcNameBuffer, sizeof(funcNameBuffer), &bytesRead);
        if (strcmp(funcName, funcNameBuffer) == 0) {
            functionRVA = funcAddrTableRVA[nameOrdinalTableRVA[i]];
            break;
        }
    }

    // 清理記憶體
    delete[] nameTableRVA;
    delete[] funcAddrTableRVA;
    delete[] nameOrdinalTableRVA;

    if (functionRVA == 0) {
        std::cerr << "找不到函數: " << funcName << std::endl;
        return 0;
    }

    // 計算 `getMyFunction` 的絕對地址
    DWORD_PTR functionAddress = (DWORD_PTR)hModule + functionRVA;
    return functionAddress;
}