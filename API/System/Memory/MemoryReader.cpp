#include "MemoryReader.h"

int MemoryReader::ReadBytes(DWORD_PTR baseAddress, BYTE* buffer, size_t size)
{
    SIZE_T bytesRead;
    if (ReadProcessMemory(ProcessInfo::hProcess, reinterpret_cast<LPCVOID>(baseAddress), buffer, size, &bytesRead))
        return bytesRead == size;
    return false;
}

DWORD_PTR MemoryReader::ReadMultiLevelPointer(DWORD_PTR baseAddress, size_t Level) {

    bool success = false;
    DWORD_PTR ptr = baseAddress;
    DWORD_PTR nextPtr = NULL;

    for (int i = 1; i <= Level; i++) {
        if (ReadMem(nextPtr, ptr)) {
            ptr = nextPtr;
            if (i == Level) success = true;
        }
        else break;
    }

    return success ? ptr : NULL;
}

bool MemoryReader::IsPointer(DWORD_PTR address)
{
    return ReadMem<DWORD_PTR>(TempDWORD, address);
}

bool MemoryReader::ReadString(DWORD_PTR baseAddress, BYTE* buffer, int strMaxLen)
{
    ReadBytes(baseAddress, buffer, strMaxLen);     // 字串最長限制為 50
    buffer[strMaxLen] = '\0';
    return true;
}

std::vector<uint8_t> MemoryReader::ReadAllBytesOfFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate); // 以二進位模式打開檔案並移到結尾
    if (!file) {
        throw std::ios_base::failure("Failed to open file.");
    }

    std::streamsize size = file.tellg(); // 取得檔案大小
    file.seekg(0, std::ios::beg); // 回到開頭

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::ios_base::failure("Failed to read file.");
    }

    return buffer; // 回傳檔案內容
}