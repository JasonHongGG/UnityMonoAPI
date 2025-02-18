#include "MemoryWriter.h"

bool MemoryWriter::WriteBytes(DWORD_PTR baseAddress, BYTE* data, size_t size)
{
    DWORD OldProtect;
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, size, PAGE_EXECUTE_READWRITE, &OldProtect);
    bool result = WriteProcessMemory(ProcessInfo::hProcess, (LPVOID)baseAddress, data, size, 0);
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, size, OldProtect, nullptr);
    return result;
}