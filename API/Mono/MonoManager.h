#pragma once
#include <vector>
#include <Windows.h>
#include "../System/Memory.h"
#include "../System/Process.h"
#include "MonoFunction.h"
#include "MonoUtils.h"
#include "MonoImage.h"
#include "MonoClass.h"

class MonoManager
{
public:
	MonoNativeFuncSet FunctSet;
	HMODULE hMonoModule = NULL;
	DWORD_PTR GetProcAddressFunctionAddress = 0x0;
	DWORD_PTR RootDomainAddress = 0x0;
	DWORD_PTR AttachAddress = 0x0;
	DWORD_PTR DetachAddress = 0x0;
	std::vector<DWORD_PTR> ThreadFunctionList = { 0, 0, 0 };

	MonoImageAPI* ImageAPI;
	MonoClassAPI* ClassAPI;
public:

	DWORD_PTR FindMonoApiAddress(DWORD_PTR AllocMemoryAddress, std::string FunctionName, HMODULE hModule = nullptr)
	{
		if (!hModule) hModule = hMonoModule;
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
		MonoUtils.PatchAddress(Code, { 6, 16, 26, 38 }, {
				(DWORD_PTR)hModule,			// 蠢传 rcx
				FunctNameAddress,				// 蠢传 rdx
				GetProcAddressFunctionAddress,  // 蠢传 rax
				ReturnAddress					// 蠢传 r12
		});

		// Execute Code
		MemMgr.MemWriter.WriteBytes(AllocMemoryAddress, Code.data(), Code.size());
		MemMgr.RegionMgr.CreateRemoteThreadAndExcute(ProcessInfo::hProcess, AllocMemoryAddress);

		DWORD_PTR FunctionAddress = 0x0;
		MemMgr.MemReader.ReadMem(FunctionAddress, ReturnAddress);
		return FunctionAddress;
	}

	DWORD_PTR GetFunctionAddress(HMODULE hModule, std::string FunctionName)
	{
		DWORD_PTR AllocMemoryAddress = MemMgr.RegionMgr.MemoryAlloc(ProcessInfo::hProcess);
		DWORD_PTR FunctionAddress = FindMonoApiAddress(AllocMemoryAddress, FunctionName, hModule);
		MemMgr.RegionMgr.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
		return FunctionAddress;
	}

	void BuildMonoFunctSet()
	{
		FunctSet = MonoNativeFuncSet();
		DWORD_PTR AllocMemoryAddress = MemMgr.RegionMgr.MemoryAlloc(ProcessInfo::hProcess);
		if (IsIL2CPP) {
			for (const std::string functName : il2cpp_native_func_name) {
				FunctSet.FunctPtrSet[il2cpp_mono_native_func_map[functName]]->FunctionAddress = FindMonoApiAddress(AllocMemoryAddress, functName);
			}
		}
		else {
			for (const std::string functName : mono_native_func_name) {
				FunctSet.FunctPtrSet[functName]->FunctionAddress = FindMonoApiAddress(AllocMemoryAddress, functName);
			}
		}
		MemMgr.RegionMgr.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
	}

	void GetRootDomain()
	{
		if(IsIL2CPP) RootDomainAddress = (DWORD_PTR)FunctSet.FunctPtrSet["mono_domain_get"]->Call<DWORD_PTR>(CALL_TYPE_CDECL);
		else RootDomainAddress = (DWORD_PTR)FunctSet.FunctPtrSet["mono_get_root_domain"]->Call<DWORD_PTR>(CALL_TYPE_CDECL);
	}

	void Init()
	{
		HMODULE hModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"KERNEL32.dll");
		GetProcAddressFunctionAddress = ProcMgr.ModuleMgr.GetFunctionAddress(ProcessInfo::hProcess, hModule, "GetProcAddress");

		hMonoModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"mono.dll");
		if (!hMonoModule) hMonoModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"mono-2.0-bdwgc.dll");
		if (!hMonoModule) { hMonoModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"GameAssembly.dll"); IsIL2CPP = true; }

		BuildMonoFunctSet();
		GetRootDomain();

		AttachAddress = FunctSet.FunctPtrSet["mono_thread_attach"]->FunctionAddress;
		DetachAddress = FunctSet.FunctPtrSet["mono_thread_detach"]->FunctionAddress;
		ThreadFunctionList = { RootDomainAddress, AttachAddress, DetachAddress };

		ImageAPI = new MonoImageAPI(&FunctSet, &ThreadFunctionList);
		ClassAPI = new MonoClassAPI(ImageAPI, &FunctSet, &ThreadFunctionList);
	}

	//void HealFeature()
	//{
	//	HealMethod->Class->Instance = CurrentPlagerInstance;
	//	HealMethod->Call<float>((float)20);
	//}

	//void TakeDamageFeature()
	//{
	//	TakeDamageMethod->Class->Instance = CurrentPlagerInstance;
	//	TakeDamageMethod->Call<float>((float)10);
	//}
};

inline MonoManager MonoMgr = MonoManager();