#pragma once
#include <vector>
#include <Windows.h>
#include "../System/Memory.h"
#include "../System/Process.h"
#include "../System/FuntionTrigger.h"
#include "MonoFunction.h"
#include "MonoUtils.h"
#include "MonoImage.h"
#include "MonoClass.h"

class MonoManager
{
public:
	MonoNativeFuncSet FunctSet;
	HMODULE hMonoModule = NULL;
	DWORD_PTR RootDomainAddress = 0x0;
	DWORD_PTR AttachAddress = 0x0;
	DWORD_PTR DetachAddress = 0x0;
	std::vector<DWORD_PTR> ThreadFunctionList = { 0, 0, 0 };

	MonoImageAPI* ImageAPI;
	MonoClassAPI* ClassAPI;
public:

	void BuildMonoFunctSet()
	{
		FunctSet = MonoNativeFuncSet();
		DWORD_PTR AllocMemoryAddress = MemMgr.RegionMgr.MemoryAlloc(ProcessInfo::hProcess);
		if (IsIL2CPP) {
			for (const std::string functName : il2cpp_native_func_name) {
				FunctSet.FunctPtrSet[il2cpp_mono_native_func_map[functName]]->FunctionAddress = FuncTrigger.FindFunctionAddress(hMonoModule, AllocMemoryAddress, functName);
			}
		}
		else {
			for (const std::string functName : mono_native_func_name) {
				FunctSet.FunctPtrSet[functName]->FunctionAddress = FuncTrigger.FindFunctionAddress(hMonoModule, AllocMemoryAddress, functName);
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

};

inline MonoManager MonoMgr = MonoManager();