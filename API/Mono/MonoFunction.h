#pragma once
#include <map>
#include <algorithm> 
#include <string>
#include <vector>
#include <Windows.h>
#include <tuple>
#include <variant>
#include "MonoConst.h"
#include "MonoIL2CPPConst.h"
#include "MonoUtils.h"
#include "../System/Memory.h"
#include "../System/Process.h"
#include "../System/FuntionTrigger.h"

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
	MonoNativeFunc(std::string functName) 
	{
		if(IsIL2CPP) this->Name = functName;//"il2cpp" + functName;
		else this->Name = functName;
	}

	template <typename T, typename... Args>
	T Call(int CallType, std::vector<DWORD_PTR> ThreadFunctionList = {0,0,0}, Args... args)
	{
		std::vector<int> property = IsIL2CPP ? il2cpp_native_func_property[Name] : mono_native_func_property[Name];
		ValueTypeEnum ReturnType = (ValueTypeEnum)property[1];
		return FuncTrigger.Call<T>(CallType, ReturnType, FunctionAddress, ThreadFunctionList, args...);
	}
};

class MonoNativeFuncSet
{
public:

	std::map<std::string, MonoNativeFunc*> FunctPtrSet;
public:
	MonoNativeFuncSet() {

		if (IsIL2CPP) {
			for (const auto& [functName, property] : il2cpp_native_func_property) {
				FunctPtrSet[functName] = new MonoNativeFunc(functName);
			}
		}
		else {
			for (const auto& [functName, property] : mono_native_func_property) {
				FunctPtrSet[functName] = new MonoNativeFunc(functName);
			}
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
