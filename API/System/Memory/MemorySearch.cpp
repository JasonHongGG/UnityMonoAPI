#include "MemorySearch.h"
#include "../../Utils/Include/BS_thread_pool.hpp"
#include "SearchStrategy/SearchStrategyFactory.h"

// ============================================================================================================================================================
//                                                                                  MemorySearch
void MemorySearch::PatternSearchThread(BYTE* bSearchData, int nSearchSize, std::vector<MEMORY_REGION>& m_vMemoryRegion, size_t Start, size_t End, bool bIsCurrProcess, int iSearchMode, bool alignSearch)
{
    //SearchMode : �ݬO�n�@�� Serch �٬O SundaySearch
    auto searchStrategy = SearchStrategyFactory::CreateStrategy(iSearchMode, alignSearch);

    for (size_t i = Start; i <= End; i++) {         // �e���B�z�ɦ� -1�A�ҥH�o��n��n���� End
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
            //���� Memmory �����e�� Read �X��
            pCurrMemoryData = new BYTE[mData.dwMemorySize];
            ZeroMemory(pCurrMemoryData, mData.dwMemorySize);    //�M��
            ReadProcessMemory(ProcessInfo::hProcess, (LPCVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);

            //�p�G�p�󵥩� 0 ���Ū���X�{���~
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
            vMemSearchResult.Add(dwOffset + mData.dwBaseAddr); //�ھ� MemFind ��� Offest�A��ܦb dwBaseAddr ���� Offset ����m���ؼСA�[�J return vector
            dwOffset += nSearchSize;
            iOffset = searchStrategy->Search(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, bSearchData, nSearchSize); //�~���U�@��
        }

        //���A��eProc �B BaseAddr����NULL�A�h�{��������ݭn demalloc �����ŧi���O����϶�
        if (!bIsCurrProcess && (pCurrMemoryData != NULL))
        {
            delete[] pCurrMemoryData;
            pCurrMemoryData = NULL;
        }
    }
}

BOOL MemorySearch::PatternSearch(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, bool alignSearch, std::vector<DWORD_PTR>& result)
{
    //�`�� : ��ŦX���� Memory �϶����X 
    // => ���j��C�Ӭ����� Memory �϶�������Ū���X�� 
    // => �z�L MemFind ��M�ؼСA�åB�^�ǩM baseAddress �� Offset
    // => �N�C�@�����G�O���� Return Vector ��
    // => �̫�N���G�^��

    //��������j�p		(���� VirtualQueryEx �����~�ɡA�����N Address �첾 Page ���j�p)
    SYSTEM_INFO sysinfo;
    ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO));
    GetSystemInfo(&sysinfo);

    //�O���Ҧ��ŦX���� dwBaseAddr�BdwMemorySize
    DWORD_PTR dwAddress = dwStartAddr;
    MEMORY_BASIC_INFORMATION mbi;
    std::vector<MEMORY_REGION> m_vMemoryRegion;
    while ((dwAddress < dwEndAddr)) // mbi ���� NULL�BdwAddress �٨S����ڡBRegionSize ���� 0
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

    // MultiThread �ǳ�
    BS::thread_pool Pool;  //BS::thread_pool pool(6) �]�m�w��֤߼�
    size_t SplitCnt = Pool.get_thread_count();
    size_t Dividend = m_vMemoryRegion.size() / SplitCnt;
    size_t Remainder = m_vMemoryRegion.size() % SplitCnt;
    vMemSearchResult.Clear();
    const BS::multi_future<void> loop_future = Pool.submit_loop<unsigned int>(0, SplitCnt,
        [this, Dividend, Remainder, bSearchData, nSearchSize, &m_vMemoryRegion, SplitCnt, bIsCurrProcess, iSearchMode, alignSearch](const unsigned int i)
        {
            size_t Start = (i < Remainder) ?
                i * Dividend + i :
                i * Dividend + Remainder;           // �p�G Idx(n) ���l�ơA�h < n ���Ҧ��l��
            // 17 / 5 = 3 ... 2
            // i = 0    => 0
            // i = 1    => 3 + 1 = 4        // + i
            // i = 2    => 6 + 2 = 8        // + i
            // i = 3    => 9 + 3 = 12       // + i
            // i = 4    => 12 + 2 = 14      // + Remainder

            size_t End = Start + ((i < Remainder) ? Dividend + 1 : Dividend) - 1;          //Array �q 0 �}�l�A�̫�n - 1;  
            PatternSearchThread(bSearchData, nSearchSize, m_vMemoryRegion, Start, End, bIsCurrProcess, iSearchMode, alignSearch);
        });
    loop_future.wait();

    // �^�ǳ̲ק�M���G
    result = vMemSearchResult.GetAll();
    return TRUE;
}

int MemorySearch::AOBSCAN(BYTE bTargetBytes[], size_t bytesSize, std::vector<DWORD_PTR>& result, bool scanAllRegion, bool alignSearch)
{
    // ScanAllRegion : �j�M�Ҧ��O����d��A�_�h Default �O Main Module �� Memory Region
    size_t moduleSize = scanAllRegion ? 0x00007fffffffffff : ProcMgr.ModuleMgr.GetModuleSize(ProcessInfo::hProcess, ProcessModule::hMainMoudle);
    DWORD_PTR startAddress = scanAllRegion ? 0x0000000000000000 : (DWORD_PTR)ProcessModule::hMainMoudle;
    DWORD_PTR endAddress = startAddress + moduleSize;
    PatternSearch(bTargetBytes, bytesSize, startAddress, endAddress, false, 2, alignSearch, result);     //�u�� 0�B2 �i�H�䴩 "??" ���� bytes ��ܦ�     �A�B 2 �����

    if (result.size() != 0) return 0;   //�^�ǧ�쪺�Ĥ@��
    else                    return -1;
}

void MemorySearch::Print(std::vector<DWORD_PTR> ScanResult) {
    printf("[ScanResultSize] %d\n", (int)ScanResult.size());
    for (size_t i = 0; i < ScanResult.size(); ++i) {
        printf("[Find Address %02d] 0x%p\n", int(i + 1), (void*)ScanResult[i]);
    }
}