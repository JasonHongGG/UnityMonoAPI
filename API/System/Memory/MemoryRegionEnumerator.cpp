#include "MemoryRegionEnumerator.h"

// ============================================================================================================================================================
//                                                              EnumProcessMemoryRegions
std::vector<MEMORY_BASIC_INFORMATION> EnumProcessMemoryRegions::enumMemoryRegions(HANDLE hProc, bool printMode)
{
    if (printMode) PrintContent(true);

    SIZE_T stSize = 0;
    PBYTE pAddress = (PBYTE)0;
    SYSTEM_INFO sysinfo;
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    std::vector<MEMORY_BASIC_INFORMATION> regions;		//Record

    //��������j�p		//�z�L Page ���j�p���o�N�i�H�����M�w StartAddress �M EndAddress
    ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO));
    GetSystemInfo(&sysinfo);

    // �P�_�u�n��e�a�}�p��̤j�a�}�N�`��
    pAddress = (PBYTE)sysinfo.lpMinimumApplicationAddress;
    while (pAddress < (PBYTE)sysinfo.lpMaximumApplicationAddress)
    {
        ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
        if (!VirtualQueryEx(hProc, pAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))	//�O�Ū�
        {
            pAddress += sysinfo.dwPageSize;
            continue;
        }
        // �C���`���֥[���s������m
        pAddress = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
        regions.push_back(mbi);

        if (printMode)  // ���L�X�i�H�ϥΪ��϶�
            if (mbi.State == MEM_FREE || mbi.State == MEM_RESERVE) PrintContent(false, mbi);

        //PrintContent(false, mbi);
    }

    return regions;
}

bool EnumProcessMemoryRegions::GetMemoryRegionSizeByAddress(HANDLE hProc, DWORD_PTR Address, size_t& Size, bool printMode)
{
    if (printMode) PrintContent(true);

    SIZE_T stSize = 0;
    PBYTE pAddress = (PBYTE)0;
    SYSTEM_INFO sysinfo;
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    std::vector<MEMORY_BASIC_INFORMATION> regions;		//Record

    //��������j�p		//�z�L Page ���j�p���o�N�i�H�����M�w StartAddress �M EndAddress
    ZeroMemory(&sysinfo, sizeof(SYSTEM_INFO));
    GetSystemInfo(&sysinfo);

    // �P�_�u�n��e�a�}�p��̤j�a�}�N�`��
    pAddress = (PBYTE)sysinfo.lpMinimumApplicationAddress;
    while (pAddress < (PBYTE)sysinfo.lpMaximumApplicationAddress)
    {
        ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
        if (!VirtualQueryEx(hProc, pAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))	//�O�Ū�
        {
            pAddress += sysinfo.dwPageSize;
            continue;
        }

        if (Address >= (DWORD_PTR)mbi.BaseAddress && Address < ((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize - 1))
        {
            Size = mbi.RegionSize;
            if (printMode)  // ���L�X�i�H�ϥΪ��϶�
                PrintContent(false, mbi);

            return true;
        }

        // �C���`���֥[���s������m
        pAddress = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
        regions.push_back(mbi);
    }

    return false;
}

uint64_t EnumProcessMemoryRegions::GetMostNearSpaceByAddress(std::vector<MEMORY_BASIC_INFORMATION> regions, uint64_t targetAddress, uint64_t requiredSize)
{
    MEMORY_BASIC_INFORMATION BestMbi;
    uint64_t BestDistance = 0xFFFFFFFFFFFFFFFF;
    uint64_t BestDiff;
    uint64_t HeadDistance = 0;
    uint64_t TailDistance = 0;
    uint64_t CompDistance = 0;
    bool HeadClosestFlag = false;
    for (auto mbi : regions)
    {
        if ((mbi.State == MEM_FREE || mbi.State == MEM_RESERVE) && (mbi.RegionSize > requiredSize)) {
            if ((uint64_t)mbi.BaseAddress > targetAddress) HeadDistance = (uint64_t)mbi.BaseAddress - targetAddress;
            else HeadDistance = targetAddress - (uint64_t)mbi.BaseAddress;

            if (((uint64_t)mbi.BaseAddress + mbi.RegionSize) > targetAddress) TailDistance = ((uint64_t)mbi.BaseAddress + mbi.RegionSize) - targetAddress;
            else TailDistance = targetAddress - ((uint64_t)mbi.BaseAddress + mbi.RegionSize);

            if (HeadDistance < TailDistance) CompDistance = HeadDistance;
            else CompDistance = TailDistance;

            if (CompDistance < BestDistance) {
                BestDistance = CompDistance;
                BestMbi = mbi;

                if (HeadDistance < TailDistance) HeadClosestFlag = true;
                else HeadClosestFlag = false;
            }
        }
    }
    //return BestMbi;
    printf("[Best MBI] [Address] %p [Size] %llX [Type] %lX [State] %lX\n", (void*)BestMbi.BaseAddress, BestMbi.RegionSize, BestMbi.Type, BestMbi.State);

    uint64_t RecommendAllocateAddress = 0;
    if (BestMbi.BaseAddress != NULL) { //�ܤ֦����@��
        if (HeadClosestFlag) {
            RecommendAllocateAddress = (uint64_t)BestMbi.BaseAddress;                                     //��K�ۡA���n���Ŷ�

            // �y�L�����@�I�A������O����Ŷ����۱�
            //if (BestDistance > 0x1000)      RecommendAllocateAddress = RecommendAllocateAddress + 0x1000;
            //else if (BestDistance > 0x100)  RecommendAllocateAddress = RecommendAllocateAddress + 0x100;
            //else if (BestDistance > 0x10)   RecommendAllocateAddress = RecommendAllocateAddress + 0x10;
        }
        else {
            RecommendAllocateAddress = (uint64_t)BestMbi.BaseAddress + BestMbi.RegionSize - requiredSize; //��K�ۡA���n���Ŷ�

            // �y�L�����@�I�A������O����Ŷ����۱�
            //if (BestDistance > 0x1000)      RecommendAllocateAddress = RecommendAllocateAddress - 0x1000;
            //else if (BestDistance > 0x100)  RecommendAllocateAddress = RecommendAllocateAddress - 0x100;
            //else if (BestDistance > 0x10)   RecommendAllocateAddress = RecommendAllocateAddress - 0x10;
        }
    }

    return RecommendAllocateAddress;
}

void EnumProcessMemoryRegions::WriteMemoryRegionsToFile(const std::vector<MEMORY_BASIC_INFORMATION>& regions, const std::string& filename) {
    std::ofstream file(filename, std::ofstream::out);	//�w�]�����N�O�л\ (std::ofstream::out)

    file << "-----------------------------------------------------------------------------------" << std::endl;
    file << "baseAddress \t\t RegionSize \t State \t\t\t ���s����" << std::endl;
    file << "-----------------------------------------------------------------------------------" << std::endl;
    for (const MEMORY_BASIC_INFORMATION& mbi : regions) {
        file << std::setw(16) << std::setfill('0') << std::hex << std::left << mbi.BaseAddress << "\t\t";
        file << std::dec << std::setw(10) << std::right << (mbi.RegionSize >> 10) << "K \t";
        switch (mbi.State)
        {
        case MEM_FREE:		file << "[10000] MEM_FREE \t\t"; break;
        case MEM_RESERVE:	file << "[  2000] MEM_RESERVE \t"; break;
        case MEM_COMMIT:	file << "[  1000] MEM_COMMIT \t"; break;
        default:			file << "[%x] \t", mbi.State; break;
        }

        switch (mbi.Type)
        {
        case MEM_PRIVATE:   file << "MEM_PRIVATE  \t"; break;
        case MEM_MAPPED:    file << "MEM_MAPPED  \t"; break;
        case MEM_IMAGE:     file << "MEM_IMAGE  \t"; break;
        default:			file << "Unknown  \t"; break;
        }

        if (mbi.Protect == 0)
            file << "---";
        else if (mbi.Protect & PAGE_EXECUTE)
            file << "E--";
        else if (mbi.Protect & PAGE_EXECUTE_READ)
            file << "ER-";
        else if (mbi.Protect & PAGE_EXECUTE_READWRITE)
            file << "ERW";
        else if (mbi.Protect & PAGE_READONLY)
            file << "-R-";
        else if (mbi.Protect & PAGE_READWRITE)
            file << "-RW";
        else if (mbi.Protect & PAGE_WRITECOPY)
            file << "WCOPY";
        else if (mbi.Protect & PAGE_EXECUTE_WRITECOPY)
            file << "EWCOPY";

        file << std::endl;
    }
    file.close();
}

void EnumProcessMemoryRegions::PrintContent(bool prologue, MEMORY_BASIC_INFORMATION mbi) {
    if (!prologue && mbi.BaseAddress != NULL)
    {
        printf("0x%p\t 0x%p\t %08llX\t ", (void*)mbi.BaseAddress, (void*)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize), mbi.RegionSize);
        switch (mbi.State)
        {
        case MEM_FREE:		printf("[10000] MEM_FREE \t"); break;
        case MEM_RESERVE:	printf("[ 2000] MEM_RESERVE \t"); break;
        case MEM_COMMIT:	printf("[ 1000] MEM_COMMIT \t"); break;
        default:			printf("[%x] \t", mbi.State); break;
        }

        switch (mbi.Type)
        {
        case MEM_PRIVATE:   printf("MEM_PRIVATE  \t"); break;
        case MEM_MAPPED:    printf("MEM_MAPPED  \t"); break;
        case MEM_IMAGE:     printf("MEM_IMAGE  \t"); break;
        default:			printf("Unknown  \t"); break;
        }

        if (mbi.Protect == 0)
            printf("---");
        else if (mbi.Protect & PAGE_EXECUTE)
            printf("E--");
        else if (mbi.Protect & PAGE_EXECUTE_READ)
            printf("ER-");
        else if (mbi.Protect & PAGE_EXECUTE_READWRITE)
            printf("ERW");
        else if (mbi.Protect & PAGE_READONLY)
            printf("-R-");
        else if (mbi.Protect & PAGE_READWRITE)
            printf("-RW");
        else if (mbi.Protect & PAGE_WRITECOPY)
            printf("WCOPY");
        else if (mbi.Protect & PAGE_EXECUTE_WRITECOPY)
            printf("EWCOPY");
        printf("\n");
    }
    else {
        printf("--------------------------------------------------------------------------------------------------- \n");
        printf("�}�l�a�} \t\t �����a�} \t\t �j�p \t\t ���A \t\t\t ���s���� \n");
        printf("--------------------------------------------------------------------------------------------------- \n");
    }
}
