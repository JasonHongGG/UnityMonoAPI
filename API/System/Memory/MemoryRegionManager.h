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

    DWORD_PTR MemoryAlloc(HANDLE procHanlder, DWORD_PTR Address = NULL, size_t size = 4096, DWORD type = MEM_COMMIT | MEM_RESERVE, DWORD protect = PAGE_EXECUTE_READWRITE);

    void MemoryFree(HANDLE procHanlder, DWORD_PTR Address);

    void CreateRemoteThreadAndExcute(HANDLE hProc, DWORD_PTR ExecuteMemoryAddress);

    void CheckMemoryProtect(HANDLE hProc, DWORD_PTR address);

    void SetExecutable(HANDLE hProc, DWORD_PTR address, size_t size = 4096);

private:
    void PrintContent(bool prologue = false, MEMORY_BASIC_INFORMATION mbi = MEMORY_BASIC_INFORMATION());
};