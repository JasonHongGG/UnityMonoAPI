#pragma once
#include <vector>
#include <Windows.h>
#include <string>

#include "Memory.h"

class FunctionTrigger
{
	DWORD_PTR GetProcAddress_FunctionAddress = NULL;
public:
	FunctionTrigger() {};
	~FunctionTrigger() {};

	void PatchAddress(std::vector<BYTE>& Code, std::vector<int> Offsets, std::vector<DWORD_PTR> Addresses) {
		for (int i = 0; i < Offsets.size(); i++) {
			int Offset = Offsets[i];
			DWORD_PTR Address = Addresses[i];

			if (Offset + 8 > Code.size()) {
				std::cerr << "Offset out of range!" << std::endl;
				return;
			}
			std::memcpy(&Code[Offset], &Address, sizeof(DWORD_PTR));
		}
	}

	void GetProcAddressFunction()
	{
		HMODULE hModule = GetModuleHandle(L"Kernel32.dll");
		GetProcAddress_FunctionAddress = (DWORD_PTR)GetProcAddress(hModule, "GetProcAddress");
	}

	DWORD_PTR FindFunctionAddress(HMODULE hModule, DWORD_PTR AllocMemoryAddress, std::string FunctionName)
	{
		if (GetProcAddress_FunctionAddress == NULL) GetProcAddressFunction();

		DWORD_PTR ReturnAddress = AllocMemoryAddress + 0x300;
		DWORD_PTR FunctNameAddress = AllocMemoryAddress + 0x200;

		// Write Function Name
		BYTE* ByteArray = reinterpret_cast<BYTE*>(FunctionName.data());
		MemMgr.MemWriter.WriteBytes(FunctNameAddress, ByteArray, FunctionName.size() + 1);

		// Prepare Code
		std::vector<BYTE> Code = {
			0x48, 0x83, 0xEC, 0x28,											// sub rsp, 28h
			0x48, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov rcx, 0xFF FF FF FF FF FF FF FF
			0x48, 0xBA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov rdx, 0xFFFFFFFFFFFFFFFF	// ボ蠢传跋办
			0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov rax, 0xFFFFFFFFFFFFFFFF
			0xFF, 0xD0,														// call rax
			0x49, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov r12, 0xFFFFFFFFFFFFFFFF
			0x49, 0x89, 0x04, 0x24,											// mov qword ptr [r12], rax
			0x48, 0x83, 0xC4, 0x28,											// add rsp, 28h
			0xC3															// ret
		};
		PatchAddress(Code, { 6, 16, 26, 38 }, {
				(DWORD_PTR)hModule,			// 蠢传 rcx
				FunctNameAddress,				// 蠢传 rdx
				GetProcAddress_FunctionAddress,  // 蠢传 rax
				ReturnAddress					// 蠢传 r12
			});

		// Execute Code
		MemMgr.MemWriter.WriteBytes(AllocMemoryAddress, Code.data(), Code.size());
		MemMgr.RegionMgr.CreateRemoteThreadAndExcute(ProcessInfo::hProcess, AllocMemoryAddress);

		DWORD_PTR FunctionAddress = 0x0;
		MemMgr.MemReader.ReadMem(FunctionAddress, ReturnAddress);
		return FunctionAddress;
	}
};

inline FunctionTrigger FuncTrigger = FunctionTrigger();