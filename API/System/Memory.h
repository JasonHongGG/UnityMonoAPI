#pragma once
#pragma warning (disable : 6001 )
#pragma warning (disable : 4244 )

#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <mutex>

#include "Process.h"
#include "../Utils/Utils.h"
#include "Memory/MemoryReader.h"
#include "Memory/MemoryWriter.h"
#include "Memory/MemorySearch.h"    
#include "Memory/MemoryRegionEnumerator.h"


class Memory {
public:
    MemoryReader MemReader;
    MemoryWriter MemWriter;
    MemorySearch Scanner;
    EnumProcessMemoryRegions RegionEnumerator;

    Memory(){
        MemReader = MemoryReader();
        MemWriter = MemoryWriter();
        Scanner = MemorySearch();
        RegionEnumerator = EnumProcessMemoryRegions();
    }
    ~Memory(){}

    void AddressToBytes(DWORD_PTR Address, BYTE BytesArray[]);

    std::vector<BYTE> HexStringToBytes(const std::string& hex);
};

inline Memory MemMgr = Memory();

