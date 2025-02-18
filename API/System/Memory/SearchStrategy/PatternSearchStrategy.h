#pragma once
#include "SearchStrategyInterface.h"

class PatternSearchStrategy : public ISearchStrategy {
public:
    PatternSearchStrategy(bool alignSearch) : alignSearch(alignSearch) {}

    int Search(BYTE* memory, size_t memorySize, BYTE* searchData, size_t searchSize) override {
        // 使用 FindPattern 實現
        return FindPattern(memory, memorySize, searchData, searchSize, alignSearch);
    }

    int FindPattern(BYTE* pBaseAddress, int size, BYTE* pbPattern, DWORD sePattern, bool alignSearch);

private:
    bool alignSearch;
};

int PatternSearchStrategy::FindPattern(BYTE* pBaseAddress, int size, BYTE* pbPattern, DWORD sePattern, bool alignSearch)
{
    auto DataCompare = [](const BYTE* pData, const BYTE* mask, BYTE chLast, size_t sePattern) -> bool {
        if (chLast != '?' && pData[sePattern - 1] != chLast) return false;
        for (int i = 0; i < sePattern; i++, ++pData, ++mask) {
            if (*pData != *mask && *mask != '?') {
                return false;
            }
        }
        return true;
        };

    int ForLoopStep = 1;
    if (alignSearch) {
        while ((reinterpret_cast<uintptr_t>(pBaseAddress) % 4) != 0) {
            pBaseAddress += 1;
        }
        ForLoopStep = 4;
    }

    BYTE chLast = pbPattern[sePattern - 1];
    //printf("Char %c", chLast);
    auto* pEnd = pBaseAddress + size - sePattern;
    for (int i = 0; i < size - (int)sePattern; i += ForLoopStep) {
        if (DataCompare(pBaseAddress + i, pbPattern, chLast, sePattern)) {
            return i;
        }
    }

    return -1;
};
