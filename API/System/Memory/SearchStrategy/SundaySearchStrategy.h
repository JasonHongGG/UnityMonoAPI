#pragma once
#include "SearchStrategyInterface.h"

class SundaySearchStrategy : public ISearchStrategy {
public:
    int Search(BYTE* memory, size_t memorySize, BYTE* searchData, size_t searchSize) override {
        return SundaySearch(memory, memorySize, searchData, searchSize);
    }

    int SundaySearch(BYTE* bStartAddr, int dwSize, BYTE* bSearchData, int dwSearchSize);
};

int SundaySearchStrategy::SundaySearch(BYTE* bStartAddr, int dwSize, BYTE* bSearchData, int dwSearchSize)
{
    if (dwSize < 0)
        return -1;

    int iIndex[256] = { 0 };
    int i, j, k;
    for (i = 0; i < 256; i++)
    {
        iIndex[i] = -1;
    }

    j = 0;
    for (i = dwSearchSize - 1; i >= 0; i--)
    {
        if (iIndex[bSearchData[i]] == -1)
        {
            iIndex[bSearchData[i]] = dwSearchSize - i;
            if (++j == 256)
                break;
        }
    }

    i = 0;
    BOOL bFind = FALSE;
    j = dwSize - dwSearchSize + 1;
    while (i < j)
    {
        for (k = 0; k < dwSearchSize; k++)
        {
            if (bStartAddr[i + k] != bSearchData[k])
                break;
        }

        if (k == dwSearchSize)
        {
            bFind = TRUE;
            break;
        }

        if (i + dwSearchSize >= dwSize)
        {

            return -1;
        }

        k = iIndex[bStartAddr[i + dwSearchSize]];
        if (k == -1)
            i = i + dwSearchSize + 1;
        else
            i = i + k;
    }

    if (bFind)
    {
        return i;
    }
    else
        return -1;
}