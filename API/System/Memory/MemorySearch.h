#pragma once
#include <Windows.h>
#include <vector>
#include "../Process.h"

class MemorySearch
{
public:
    MemorySearch() {};
    ~MemorySearch() {};

    typedef struct _MEMORY_REGION
    {
        DWORD_PTR dwBaseAddr;
        DWORD_PTR dwMemorySize;

    } MEMORY_REGION, * PMEMORY_REGION;

    // Pattern Search
    void PatternSearchThread(BYTE* bSearchData, int nSearchSize, std::vector<MEMORY_REGION>& m_vMemoryRegion, size_t Start, size_t End, bool bIsCurrProcess, int iSearchMode, bool alignSearch);

    BOOL PatternSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, bool alignSearch, std::vector<DWORD_PTR>& result);

    int AOBSCAN(BYTE bTargetBytes[], size_t bytesSize, std::vector<DWORD_PTR>& result, bool scanAllRegion = false, bool alignSearch = false);

    void Print(std::vector<DWORD_PTR> ScanResult);
private:
};

class ThreadSafeStorageClass {
public:
    ThreadSafeStorageClass() {};
    ~ThreadSafeStorageClass() {};

    void Add(const DWORD_PTR& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        storage_.push_back(item);
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        storage_.clear();
    }

    std::vector<DWORD_PTR> GetAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        return storage_;
    }

private:
    std::vector<DWORD_PTR> storage_;
    std::mutex mutex_;
};

inline ThreadSafeStorageClass vMemSearchResult = ThreadSafeStorageClass();