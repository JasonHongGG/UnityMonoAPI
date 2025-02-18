#pragma once
#include "SearchStrategyInterface.h"

class NormalSearchStrategy : public ISearchStrategy {
public:
    int Search(BYTE* memory, size_t memorySize, BYTE* searchData, size_t searchSize) override {
        return MemSearch(memory, memorySize, searchData, searchSize);
    }

    int MemSearch(BYTE* buffer, int dwBufferSize, BYTE* bstr, int dwStrLen);
};


int NormalSearchStrategy::MemSearch(BYTE* buffer, int dwBufferSize, BYTE* bstr, int dwStrLen)
{
    if (dwBufferSize < 0) return -1;

    int  i, j;
    for (i = 0; i < dwBufferSize; i++)
    {
        for (j = 0; j < dwStrLen; j++)
        {
            if (buffer[i + j] != bstr[j] && bstr[j] != '?') break;
        }

        if (j == dwStrLen) return i;
    }

    return -1;
}