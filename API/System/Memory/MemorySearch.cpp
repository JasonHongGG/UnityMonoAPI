#include "MemorySearch.h"
#include "../../Utils/Include/BS_thread_pool.hpp"
#include "SearchStrategy/SearchStrategyFactory.h"

// ============================================================================================================================================================
//                                                                                  MemorySearch
void MemorySearch::PatternSearchThread(BYTE* bSearchData, int nSearchSize, std::vector<MEMORY_REGION>& m_vMemoryRegion, size_t Start, size_t End, bool bIsCurrProcess, int iSearchMode, bool alignSearch)
{
    //SearchMode : 看是要一般 Serch 還是 SundaySearch
    auto searchStrategy = SearchStrategyFactory::CreateStrategy(iSearchMode, alignSearch);

    for (size_t i = Start; i <= End; i++) {         // 前面處理時有 -1，所以這邊要剛好做到 End
        MEMORY_REGION mData = m_vMemoryRegion[i];
        DWORD_PTR dwNumberOfBytesRead = 0;
        BYTE* pCurrMemoryData = NULL;

        if (bIsCurrProcess)
        {
            pCurrMemoryData = (BYTE*)mData.dwBaseAddr;
            dwNumberOfBytesRead = mData.dwMemorySize;
        }
        else
        {
            //把整塊 Memmory 的內容都 Read 出來
            pCurrMemoryData = new BYTE[mData.dwMemorySize];
            ZeroMemory(pCurrMemoryData, mData.dwMemorySize);    //清空
            ReadProcessMemory(ProcessInfo::hProcess, (LPCVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);

            //如果小於等於 0 表示讀取出現錯誤
            if ((int)dwNumberOfBytesRead <= 0)
            {
                delete[] pCurrMemoryData;
                continue;
            }
        }
        
        DWORD_PTR dwOffset = 0;
        int iOffset = searchStrategy->Search(pCurrMemoryData, dwNumberOfBytesRead, bSearchData, nSearchSize);
        while (iOffset != -1) {
            dwOffset += iOffset;
            vMemSearchResult.Add(dwOffset + mData.dwBaseAddr); //根據 MemFind 找到 Offest，表示在 dwBaseAddr 偏移 Offset 的位置找到目標，加入 return vector
            dwOffset += nSearchSize;
            iOffset = searchStrategy->Search(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, bSearchData, nSearchSize); //繼續找下一個
        }

        //不再當前Proc 且 BaseAddr不為NULL，則程式結束後需要 demalloc 相關宣告的記憶體區塊
        if (!bIsCurrProcess && (pCurrMemoryData != NULL))
        {
            delete[] pCurrMemoryData;
            pCurrMemoryData = NULL;
        }
    }
}

BOOL MemorySearch::PatternSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, bool alignSearch, std::vector<DWORD_PTR>& result)
{
    //總結 : 把符合條件的 Memory 區塊取出 
    // => 遞迴把每個紀錄的 Memory 區塊整塊資料讀取出來 
    // => 透過 MemFind 找尋目標，並且回傳和 baseAddress 的 Offset
    // => 將每一筆結果記錄到 Return Vector 內
    // => 最後將結果回傳

    //獲取頁的大小		(當之後 VirtualQueryEx 有錯誤時，直接將 Address 位移 Page 的大小)
    SYSTEM_INFO sysinfo;
    ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO));
    GetSystemInfo(&sysinfo);

    //記錄所有符合條件的 dwBaseAddr、dwMemorySize
    DWORD_PTR dwAddress = dwStartAddr;
    MEMORY_BASIC_INFORMATION mbi;
    std::vector<MEMORY_REGION> m_vMemoryRegion;
    while ((dwAddress < dwEndAddr)) // mbi 不為 NULL、dwAddress 還沒到尾巴、RegionSize 不為 0
    {
        ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
        if (!VirtualQueryEx(ProcessInfo::hProcess, (LPCVOID)dwAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION))) {
            dwAddress += sysinfo.dwPageSize;
            continue;
        }

        if ((mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE))
        {

            MEMORY_REGION mData = { 0 };
            mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
            mData.dwMemorySize = mbi.RegionSize;
            m_vMemoryRegion.push_back(mData);
            //printf("[dwBaseAddr] %p \t [Size] %7X [mbi.State] %X\n", mbi.baseAddress, mbi.RegionSize, mbi.State);
        }

        dwAddress = (DWORD_PTR)mbi.BaseAddress + mbi.RegionSize;
        if (mbi.RegionSize == 0) break;
    }

    // MultiThread 準備
    BS::thread_pool Pool;  //BS::thread_pool pool(6) 設置硬體核心數
    size_t SplitCnt = Pool.get_thread_count();
    size_t Dividend = m_vMemoryRegion.size() / SplitCnt;
    size_t Remainder = m_vMemoryRegion.size() % SplitCnt;
    vMemSearchResult.Clear();
    const BS::multi_future<void> loop_future = Pool.submit_loop<unsigned int>(0, SplitCnt,
        [this, Dividend, Remainder, bSearchData, nSearchSize, &m_vMemoryRegion, SplitCnt, bIsCurrProcess, iSearchMode, alignSearch](const unsigned int i)
        {
            size_t Start = (i < Remainder) ?
                i * Dividend + i :
                i * Dividend + Remainder;           // 如果 Idx(n) 有餘數，則 < n 的皆有餘數
            // 17 / 5 = 3 ... 2
            // i = 0    => 0
            // i = 1    => 3 + 1 = 4        // + i
            // i = 2    => 6 + 2 = 8        // + i
            // i = 3    => 9 + 3 = 12       // + i
            // i = 4    => 12 + 2 = 14      // + Remainder

            size_t End = Start + ((i < Remainder) ? Dividend + 1 : Dividend) - 1;          //Array 從 0 開始，最後要 - 1;  
            PatternSearchThread(bSearchData, nSearchSize, m_vMemoryRegion, Start, End, bIsCurrProcess, iSearchMode, alignSearch);
        });
    loop_future.wait();

    // 回傳最終找尋結果
    result = vMemSearchResult.GetAll();
    return TRUE;
}

int MemorySearch::AOBSCAN(BYTE bTargetBytes[], size_t bytesSize, std::vector<DWORD_PTR>& result, bool scanAllRegion, bool alignSearch)
{
    // ScanAllRegion : 搜尋所有記憶體範圍，否則 Default 是 Main Module 的 Memory Region
    size_t moduleSize = scanAllRegion ? 0x00007fffffffffff : ProcMgr.ModuleMgr.GetModuleSize(ProcessInfo::hProcess, ProcessModule::hMainMoudle);
    DWORD_PTR startAddress = scanAllRegion ? 0x0000000000000000 : (DWORD_PTR)ProcessModule::hMainMoudle;
    DWORD_PTR endAddress = startAddress + moduleSize;
    PatternSearch(bTargetBytes, bytesSize, startAddress, endAddress, false, 2, alignSearch, result);     //只有 0、2 可以支援 "??" 未知 bytes 表示式     ，且 2 比較快

    if (result.size() != 0) return 0;   //回傳找到的第一個
    else                    return -1;
}

void MemorySearch::Print(std::vector<DWORD_PTR> ScanResult) {
    printf("[ScanResultSize] %d\n", (int)ScanResult.size());
    for (size_t i = 0; i < ScanResult.size(); ++i) {
        printf("[Find Address %02d] 0x%p\n", int(i + 1), (void*)ScanResult[i]);
    }
}