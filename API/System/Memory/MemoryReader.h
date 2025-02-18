#pragma once
#include <Windows.h>
#include "../Process.h"

class MemoryReader {
public:
    DWORD_PTR TempDWORD = NULL;

    template <class T>
    bool ReadMem(T& target, DWORD_PTR baseAddress, size_t size = -1);

    int ReadBytes(DWORD_PTR baseAddress, BYTE* buffer, size_t size);

    DWORD_PTR ReadMultiLevelPointer(DWORD_PTR baseAddress, size_t Level);

	template <class T>
    std::vector<T> ReadArray(DWORD_PTR baseAddress, size_t n);

    bool ReadString(DWORD_PTR baseAddress, BYTE* buffer, int strMaxLen = 50);

    bool IsPointer(DWORD_PTR address);

    template <class T>
    bool IsEqual(DWORD_PTR address, T value);
private:
};

template <class T>
std::vector<T> MemoryReader::ReadArray(DWORD_PTR baseAddress, size_t n)
{
    std::vector<T> result;
    for (int i = 0; i < n; i++) {
        T temp;
        ReadMem(temp, baseAddress + i * sizeof(T));
        result.push_back(temp);
    }

    return result;
}

template <class T>
bool MemoryReader::ReadMem(T& target, DWORD_PTR baseAddress, size_t size) 
{
    if (size == -1) size = sizeof(target);
    if (ReadProcessMemory(ProcessInfo::hProcess, reinterpret_cast<LPCVOID>(baseAddress), &target, size, NULL) == FALSE) return false; // 回傳預設值和失敗標誌
    return true;
}

template <class T>
bool MemoryReader::IsEqual(DWORD_PTR address, T value)
{
    T result;
    ReadMem(result, address);
    return result == value;
}