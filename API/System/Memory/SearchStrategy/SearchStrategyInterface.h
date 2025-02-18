#pragma once
#include <Windows.h>

class ISearchStrategy {
public:
    virtual ~ISearchStrategy() = default;
    virtual int Search(BYTE* memory, size_t memorySize, BYTE* searchData, size_t searchSize) = 0;
};

