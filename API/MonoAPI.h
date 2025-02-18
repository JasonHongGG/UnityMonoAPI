// UnityMonoAPI.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include "System/Memory.h"
#include "System/Process.h"
#include "Mono/MonoManager.h"
#include "Utils/Utils.h"


void InitialProcess(std::wstring wProcessName)
{
	// initial
	//StorageMgr.SetUEVersion(0);
	//StorageMgr.SetGWorld(0);
	//StorageMgr.SetGUObjectArray(0);
	//StorageMgr.SetFNamePool(0);

	// ==========  Initial String Parameter  ==========
	std::wstring WindowName;
	ProcessInfo::ProcessName = Utils.UnicodeToUTF8(wProcessName.c_str());

	// ==========  Get Process ID、Handler  ==========
	if (!ProcMgr.InfoMgr.GetPID(Utils.UTF8ToUnicode(ProcessInfo::ProcessName.c_str()).c_str())) {
		printf("Process Not Running !!!");
		exit(0);
	}

	if (ProcessInfo::hProcess != 0) CloseHandle(ProcessInfo::hProcess);		// 如果要換 Process，要把前一個 Process Handler 關閉
	ProcessInfo::hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)ProcessInfo::PID);
	ProcMgr.InfoMgr.ProcessIs64Bit();
	printf("[ Process ID ] %p\n", (void*)ProcessInfo::PID);
	printf("[ Process Handler ] %p\n", (void*)ProcessInfo::hProcess);
	printf("\n");


	// ==========  Get Module、Module Base  ==========
	ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, wProcessName.c_str());
	DWORD_PTR base = ProcMgr.ModuleMgr.GetModuleBaseAddress(ProcessInfo::PID, wProcessName.c_str());
	DWORD size = ProcMgr.ModuleMgr.GetModuleSize(ProcessInfo::hProcess, ProcessModule::hMainMoudle);
	printf("[ Module Obj Address ] %p\n", ProcessModule::hMainMoudle);		// module handler 取得的就是 module base address
	printf("[ Module Base Address ] %p\n", (void*)base);
	printf("[ Module Size ] %X\n", size);
	printf("\n");


	// ==========  Test Area  ==========
	MonoMgr.Init();
}