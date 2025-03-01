#pragma once
#include <Windows.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>
#include "../Process.h"

class MemoryRegionManager
{
public:
    MemoryRegionManager() {};
    ~MemoryRegionManager() {};

    std::vector<MEMORY_BASIC_INFORMATION> enumMemoryRegions(HANDLE hProc, bool printMode = false);

    // 根據 Address 找到該塊 malloc 的 memory region
    bool GetMemoryRegionSizeByAddress(HANDLE hProc, DWORD_PTR Address, size_t& Size, bool printMode = false);

    uint64_t GetMostNearSpaceByAddress(std::vector<MEMORY_BASIC_INFORMATION> regions, uint64_t TargetAddress, uint64_t requiredSize);

    void WriteMemoryRegionsToFile(const std::vector<MEMORY_BASIC_INFORMATION>& regions, const std::string& filename);

    DWORD_PTR MemoryAlloc(HANDLE procHanlder, DWORD_PTR Address = NULL, size_t size = 4096, DWORD type = MEM_COMMIT | MEM_RESERVE, DWORD protect = PAGE_EXECUTE_READWRITE)
    {
        LPVOID pMemory = VirtualAllocEx(
                            procHanlder, // 當前進程
                            (LPVOID)Address,                // 讓系統選擇地址
                            size,                // 內存大小
                            type, // 分配並提交
                            protect       // 可讀寫
                        );

        return (DWORD_PTR)pMemory;
    }

	void MemoryFree(HANDLE procHanlder, DWORD_PTR Address)
	{
		VirtualFreeEx(procHanlder, LPVOID(Address), 0, MEM_RELEASE);
	}

    void CreateRemoteThreadAndExcute(HANDLE hProc, DWORD_PTR ExecuteMemoryAddress)
    {
        DWORD OldProtect;
        VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)ExecuteMemoryAddress, 4096, PAGE_EXECUTE_READWRITE, &OldProtect);
        HANDLE hThread = CreateRemoteThread(hProc,  NULL, 0, (LPTHREAD_START_ROUTINE)ExecuteMemoryAddress,NULL, 0, NULL );
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    void CheckMemoryProtect(DWORD_PTR address)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQueryEx(ProcessInfo::hProcess, (LPCVOID)address, &mbi, sizeof(mbi))) {
            std::cout << "Memory State: " << mbi.State << std::endl;
            std::cout << "Memory Protect: " << mbi.Protect << std::endl;
        }
    }

	void SetExecutable(DWORD_PTR address, size_t size = 4096)
	{
		DWORD OldProtect;
		VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &OldProtect);
	}

private:
    void PrintContent(bool prologue = false, MEMORY_BASIC_INFORMATION mbi = MEMORY_BASIC_INFORMATION());
};