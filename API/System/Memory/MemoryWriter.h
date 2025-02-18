#pragma once
#include <Windows.h>
#include "../Process.h"

class MemoryWriter {
public:
    template <class T>
    bool WriteMem(DWORD_PTR baseAddress, T data);

    template <class T>
    void WriteProtectedMem(DWORD_PTR baseAddress, T data);

    bool WriteBytes(DWORD_PTR baseAddress, BYTE* data, size_t size);

    template <class T>
    void WriteArray(DWORD_PTR baseAddress, const std::vector<T>& data);
private:
};

template <class T>
void MemoryWriter::WriteArray(DWORD_PTR baseAddress, const std::vector<T>& data)
{
    for (size_t i = 0; i < data.size(); i++) {
        WriteMem(baseAddress + i * sizeof(T), data[i]);
    }
}

template <class T>
bool MemoryWriter::WriteMem(DWORD_PTR baseAddress, T data)
{
    return WriteProcessMemory(ProcessInfo::hProcess, (LPBYTE*)baseAddress, &data, sizeof(data), NULL);
}

template <class T>
void MemoryWriter::WriteProtectedMem(DWORD_PTR baseAddress, T data)
{
    DWORD OldProtect;
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, sizeof(data), PAGE_EXECUTE_READWRITE, &OldProtect);
    WriteProcessMemory(ProcessInfo::hProcess, (LPVOID)baseAddress, &data, sizeof(data), nullptr);
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, sizeof(data), OldProtect, nullptr);
}


