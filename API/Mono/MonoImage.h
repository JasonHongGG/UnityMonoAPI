#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "Const.h"
#include "MonoFunction.h"

class MonoAssemblyImage
{
public:
	DWORD_PTR Handle = 0x0;
public:
	MonoAssemblyImage(DWORD_PTR handle) : Handle(handle) {};
};

class MonoImage
{
public:
	DWORD_PTR AssemblyImageHandle = 0x0;
	DWORD_PTR Handle = 0x0;
	DWORD_PTR ImagesAddress = 0x0;
	std::string Name = "";
	MonoImage(MonoAssemblyImage AssemblyImage, std::string name, DWORD_PTR address) : AssemblyImageHandle(AssemblyImage.Handle), Name(name), Handle(address) {};
};

class MonoImageAPI
{
	MonoNativeFuncSet* FunctSet;
	std::vector<DWORD_PTR>* ThreadFunctionList; 
public:
	MonoImageAPI(MonoNativeFuncSet* functSet, std::vector<DWORD_PTR>* threadFunctionList) : FunctSet(functSet), ThreadFunctionList(threadFunctionList){};

	std::vector<MonoAssemblyImage> EnumAssemblies()
	{
		if (IsIL2CPP)
		{
			CValue Cnt = CValue<int>(0);
			DWORD_PTR Address = FunctSet->FunctPtrSet["il2cpp_domain_get_assemblies"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, ThreadFunctionList->at(0), Cnt.Address);
			int AssemblyCnt = Cnt.GetValue();
			std::vector<DWORD_PTR> AssemblyList = MemMgr.MemReader.ReadArray<DWORD_PTR>(Address, AssemblyCnt);

			// Create Assembly Image
			std::vector<MonoAssemblyImage> AssemblyImageList;
			for (int i = 0; i < AssemblyList.size(); i++) {
				AssemblyImageList.push_back(MonoAssemblyImage(AssemblyList[i]));
			}

			return AssemblyImageList;
		}
		else 
		{
			std::vector<BYTE> EnumAssemblyCallback = {
			0x8b,0x02,0x3d,0xfe,0x01,0x00,0x00,0x77,0x07,0x48,0x89,0x4c,0xc2,0x08,0xff,0x02,0xc3
			};
			/*
			void _cdecl enum_assembly_callback(void* domain, CUSTOM_DOMAIN_ARRAY64 * v)
			{
				if (v->cnt <= 510) {
					v->domains[v->cnt] = (UINT64)domain;
					v->cnt += 1;
				}
			}
			*/

			// Alloc Memory
			CValue Cnt = CValue<int>(0);
			DWORD_PTR CallBackAddress = MemMgr.RegionMgr.MemoryAlloc(ProcessInfo::hProcess);

			// Init
			MemMgr.MemWriter.WriteBytes(CallBackAddress, EnumAssemblyCallback.data(), EnumAssemblyCallback.size());

			// Read Assembly List
			FunctSet->FunctPtrSet["mono_assembly_foreach"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, CallBackAddress, Cnt.Address);
			int AssemblyCnt = Cnt.GetValue();
			std::vector<DWORD_PTR> AssemblyList = MemMgr.MemReader.ReadArray<DWORD_PTR>(Cnt.Address + ProcessInfo::ProcOffestAdd, AssemblyCnt);

			// Create Assembly Image
			std::vector<MonoAssemblyImage> AssemblyImageList;
			for (int i = 0; i < AssemblyList.size(); i++) {
				AssemblyImageList.push_back(MonoAssemblyImage(AssemblyList[i]));
			}

			// Free Memory
			MemMgr.RegionMgr.MemoryFree(ProcessInfo::hProcess, CallBackAddress);

			return AssemblyImageList;
		}
	}

	MonoImage* FindImageByName(std::string ImageName)
	{
		std::vector<MonoAssemblyImage> Assemblies = EnumAssemblies();
		for (MonoAssemblyImage& AssemblyImage : Assemblies) {
			DWORD_PTR ImageAddress = FunctSet->FunctPtrSet["mono_assembly_get_image"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, AssemblyImage.Handle);
			ImageAddress &= 0xFFFFFFFFFFFF; // 12 bytes
			std::string Name = FunctSet->FunctPtrSet["mono_image_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, ImageAddress);

			if (ImageName == Name)
				return new MonoImage(AssemblyImage, Name, ImageAddress);
		}
		return nullptr;
	}

	std::vector<MonoImage*> FindImagesByName(std::vector<std::string> ImageNames)
	{
		std::vector<MonoImage*> ResultImages;
		std::vector<MonoAssemblyImage> Assemblies = EnumAssemblies();
		for (MonoAssemblyImage& AssemblyImage : Assemblies) {
			DWORD_PTR ImageAddress = FunctSet->FunctPtrSet["mono_assembly_get_image"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, AssemblyImage.Handle);
			ImageAddress &= 0xFFFFFFFFFFFF; // 12 bytes
			std::string Name = FunctSet->FunctPtrSet["mono_image_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, ImageAddress);

			for (int i = 0; i < ImageNames.size(); i++) {
				if (ImageNames[i] == Name)
					ResultImages.push_back(new MonoImage(AssemblyImage, Name, ImageAddress));
			}
		}
		return ResultImages;
	}

	MonoImage* OpenImageFromData(std::vector<uint8_t> data)
	{
		std::string dataStr(data.begin(), data.end());
		CString DataStrObject(dataStr);
		CValue Status(0);

		DWORD_PTR ImageAddress = FunctSet->FunctPtrSet["mono_image_open_from_data"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, DataStrObject.Address, dataStr.size(), 1, Status.Address);
		return new MonoImage(MonoAssemblyImage(0), "Data", ImageAddress);
	}
};