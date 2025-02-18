#pragma once
#include <map>
#include <algorithm> 
#include <string>
#include <vector>
#include <Windows.h>
#include <tuple>
#include <variant>
#include "MonoConst.h"
#include "MonoUtils.h"
#include "../System/Memory.h"
#include "../System/Process.h"

class MonoNativeFunc
{
public:
	std::string Name = "";
	DWORD_PTR FunctionAddress = 0x0;
	std::vector<DWORD_PTR> _user_data;
	using ReturnType = std::variant<
		bool, char, unsigned char,
		int16_t, uint16_t, int32_t, uint32_t,
		int64_t, uint64_t, float, double,
		void*, std::string
	>;
public:
	MonoNativeFunc(std::string functName) : Name(functName){}

	template <typename T, typename... Args>
	T Call(int CallType, std::vector<DWORD_PTR> ThreadFunctionList = {0,0,0}, Args... args)
	{
		/*
		Ч俱舱粂ē
		// sub rsp, 28h
		// mov rcx, RootDomainAddress
		// mov rax, AttachAddress
		// call rax
		// mov r12, ThreadAddress
		// mov qword ptr [r12], rax
		// mov rcx, arg1
		// mov rdx, arg2
		// mov r8, arg3
		// mov r9, arg4
		// mov rax, arg5 6 7 8 9 ...
		// mov qword ptr [rsp+0x20 + (0x8 * n)], rax
		// mov rax, FunctionAddress
		// call rax
		// mov r12, ReturnAddress
		// mov qword ptr [r12], rax
		// mov r12, ThreadAddress
		// mov rcx, qword ptr [r12]
		// mov rax, DetachAddress
		// call rax
		// add rsp, 28h
		// ret
		*/
		DWORD_PTR RootDomainAddress = ThreadFunctionList[0];
		DWORD_PTR AttachAddress = ThreadFunctionList[1];
		DWORD_PTR DetachAddress = ThreadFunctionList[2];
		std::vector<DWORD_PTR> argList = { static_cast<DWORD_PTR>(args)... };
		std::vector<size_t> argSizeList = { sizeof(args) ... };
		std::vector<bool> isFloatOrDouble = { std::is_floating_point_v<Args>... };
			

		std::vector<int> property = mono_native_func_property[Name];
		int ArgsCnt = argList.size();
		ValueTypeEnum ReturnType = (ValueTypeEnum)property[1];

		DWORD_PTR AllocMemoryAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);
		DWORD_PTR ReturnAddress = AllocMemoryAddress + 0x300;
		DWORD_PTR ThreadAddress = AllocMemoryAddress + 0x400;

		std::vector<BYTE> Code;
		std::vector<BYTE> TempCode;
		BYTE ParameterStackSize = 0x28;
		if(argList.size() > 4)
			ParameterStackSize += ((argList.size() - 4 + 1) / 2) * 0x10;
		Code.insert(Code.end(), { 0x48, 0x83, 0xEC, ParameterStackSize }); // sub rsp, 28h

		// Is Thread then call Attach
		if (RootDomainAddress and AttachAddress and DetachAddress) {
			TempCode = {
				0x48, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov rcx, RootDomainAddress
				0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov rax, AttachAddress
				0xFF, 0xD0,                                                     // call rax
				0x49, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov r12, ThreadAddress
				0x49, 0x89, 0x04, 0x24,                                         // mov qword ptr [r12], rax
			};
			MonoUtils.PatchAddress(TempCode, { 2, 12, 24 }, {
				RootDomainAddress,		// 蠢传 rcx
				AttachAddress,			// 蠢传 rdx
				ThreadAddress,			// 蠢传 r12
			});
			Code.insert(Code.end(), TempCode.begin(), TempCode.end());
		}

		// Prepare Arguments
		int RegisterCnt = 0;
		size_t registerArgsCount = min(argList.size(), (size_t)4);
		std::vector<BYTE> integerRegsByte = { 0xB9, 0xBA, 0xB8, 0xB9 };	//  "rcx", "rdx", "r8", "r9"
		std::vector<BYTE> integer32bitsRegsByte = { 0xC1, 0xC2, 0xC0, 0xC1 };
		std::vector<BYTE> floatRegsByte = { 0xC0, 0xC8, 0xD0, 0xD8 }; // xmm0, xmm1, xmm2, xmm3
		for (size_t i = 0; i < registerArgsCount; i++) {
			if (isFloatOrDouble[i]) {
				TempCode = { 0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0x90, 0x90, 0x90, 0x90 };
				if (argSizeList[i] <= 4)
					MonoUtils.PatchParameter(TempCode, 2, (float)argList[i], argSizeList[i], 0xC0);
				else 
					MonoUtils.PatchParameter(TempCode, 2, (double)argList[i], argSizeList[i], 0xC8);
				Code.insert(Code.end(), TempCode.begin(), TempCode.end());

				if (argSizeList[i] <= 4) 
					TempCode = { 0x66, 0x0F, 0x6E, (BYTE)floatRegsByte[RegisterCnt]};
				else 
					TempCode = { 0x66, 0x48, 0x0F, 0x6E, (BYTE)floatRegsByte[RegisterCnt] };
				Code.insert(Code.end(), TempCode.begin(), TempCode.end());
				RegisterCnt++;
			}
			else {
				TempCode = { ((RegisterCnt <= 1) ? (BYTE)0x48 : (BYTE)0x49), (BYTE)integerRegsByte[RegisterCnt], 0xFF, 0xFF, 0xFF, 0xFF, 0x90, 0x90, 0x90, 0x90 }; // mov rcx | rdx, 0x7ffd83c60430
				MonoUtils.PatchParameter(TempCode, 2, argList[i], argSizeList[i], (BYTE)integer32bitsRegsByte[RegisterCnt]);
				Code.insert(Code.end(), TempCode.begin(), TempCode.end());
				RegisterCnt++;
			}
		}

		for (size_t i = 4; i < argList.size(); ++i) {
			TempCode = { 
				0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // mov rax, 0x210a4762d20
				0x48, 0x89, 0x44, 0x24, (BYTE)(0x20 + (i - 4) * 8),			 // mov qword ptr [rsp+0x20], rax
			};
			MonoUtils.PatchAddress(TempCode, { 2 }, { argList[i]});
			Code.insert(Code.end(), TempCode.begin(), TempCode.end());
		}

		// Call Function
		TempCode = {
			0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov rax, FunctionAddress
			0xFF, 0xD0,                                                     // call rax
			0x49, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov r12, ReturnAddress
			0x49, 0x89, 0x04, 0x24,                                         // mov qword ptr [r12], rax                             
		};
		MonoUtils.PatchAddress(TempCode, { 2, 14 }, {
			FunctionAddress,		// 蠢传 rax
			ReturnAddress,			// 蠢传 r12
		});
		Code.insert(Code.end(), TempCode.begin(), TempCode.end());


		// Is Thread then call Detach
		if (RootDomainAddress and AttachAddress and DetachAddress) {
			TempCode = {
				0x49, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov r12, ThreadAddress
				0x49, 0x8B, 0x0C, 0x24,                                         // mov rcx, qword ptr [r12]
				0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,     // mov rax, DetachAddress
				0xFF, 0xD0,                                                     // call rax
			};
			MonoUtils.PatchAddress(TempCode, { 2, 16 }, {
				ThreadAddress,		// 蠢传 r12
				DetachAddress,		// 蠢传 rax
			});
			Code.insert(Code.end(), TempCode.begin(), TempCode.end());
		}

		Code.insert(Code.end(), {
			0x48, 0x83, 0xC4, ParameterStackSize,               // add rsp, 28h
			0xC3                                                // ret
		});

		BYTE* ByteArray = reinterpret_cast<BYTE*>(Code.data());
		MemMgr.MemWriter.WriteBytes(AllocMemoryAddress, ByteArray, Code.size());
		MemMgr.RegionEnumerator.CreateRemoteThreadAndExcute(ProcessInfo::hProcess, AllocMemoryAddress);
		
		// Read Result
		T result = MonoUtils.ReadValue<T>(ReturnType, ReturnAddress);
		

		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
		return result;
	}
};

class MonoNativeFuncSet
{
public:
	std::map<std::string, MonoNativeFunc*> FunctPtrSet;
public:
	MonoNativeFuncSet() {
		for (const auto& [functName, property] : mono_native_func_property) {
			FunctPtrSet[functName] = new MonoNativeFunc(functName);
		}
	}

	static bool NativeFunctionExist(MonoNativeFuncSet* FunctSet, std::string FunctionName)
	{
		if (FunctSet->FunctPtrSet.find(FunctionName) != FunctSet->FunctPtrSet.end())
			if (FunctSet->FunctPtrSet[FunctionName]->FunctionAddress != 0x0)
				return true;
		return false;
	}
};
