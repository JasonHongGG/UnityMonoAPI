#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <ranges>
#include "MonoImage.h"
#include "MonoUtils.h"
#include "MonoConst.h"
#include "MonoFunction.h"

class MonoClassAPI;
class MonoClass;
class MonoField;
class MonoMethod;

class MonoMethod
{
public:
	DWORD_PTR Handle = 0x0;
	MonoClass* Class = nullptr;
	std::string Name = "";
	int Flags = 0;
	int ParamCnt = 0;
	std::string Signature = "";
	DWORD_PTR FunctionAdress = 0x0;
	MonoMethod(MonoClass* _class, DWORD_PTR handle, std::string name) : Class(_class), Handle(handle), Name(name) {}

	
	template <typename T, typename... Args>
	T Call(Args... args);

	DWORD_PTR GetAddress();

	std::string GetSignature();

	std::string GetReturnTypeName();

	int GetFlags();

	bool IsStatic(MonoMethod* Field);
};

class MonoField
{
public:
	DWORD_PTR Handle = 0x0;
	MonoClass* Class = nullptr;
	std::string Name = "";
	int Flags = 0;
	DWORD_PTR TypeAddress = 0;
	std::string TypeName = "";
	int Offset = 0;
	DWORD_PTR Address = 0x0;

	MonoField(MonoClass* _class, DWORD_PTR handle, std::string name) : Class(_class), Handle(handle), Name(name) {}

	int GetFlags();

	DWORD_PTR GetTypeAddress();

	std::string GetTypeName();

	DWORD_PTR GetAddress();

	template <typename T>
	T GetValue();

	DWORD_PTR GetInstance();

	int GetOffset();

	bool IsStatic(MonoField* Field);
};




class MonoClass
{
public:
	MonoClassAPI* ClassAPI;
	DWORD_PTR Handle = 0x0;
	MonoImage* Image = nullptr;
	DWORD_PTR Instance = 0x0;
	DWORD_PTR VTable = 0x0;
	std::string ClassName = "";
	std::string ClassNamespace = "";

	MonoClass(MonoClassAPI* api, MonoImage* image, DWORD_PTR handle, std::string className, std::string classNamespace)
		: ClassAPI(api), Image(image), Handle(handle), ClassName(className), ClassNamespace(classNamespace) {
	}

	MonoField* FindField(std::string FieldName);

	DWORD_PTR GetVtable();

	MonoMethod* FindMethod(std::string MethodName, int ParaCnt = -1);
};




class MonoClassAPI
{
public:
	MonoImageAPI* ImageAPI;
	MonoNativeFuncSet* FunctSet;
	std::vector<DWORD_PTR>* ThreadFunctionList;

	MonoClassAPI(MonoImageAPI* imageAPI, MonoNativeFuncSet* functSet, std::vector<DWORD_PTR>* threadFunctionList)
		: ImageAPI(imageAPI), FunctSet(functSet), ThreadFunctionList(threadFunctionList) {
	}

	MonoClass* GetClassByImage(MonoImage* Image, std::string ClassName);

	MonoClass* FindClassInImageByName(std::string ImageName, std::string ClassName);

	std::vector<MonoClass*> FindClassesInImageByName(std::string ImageName, std::vector<std::string> ClassNames);

	std::map<std::string, std::vector<MonoClass*>> FindClassesInImageByNames(std::map<std::string, std::vector<std::string>> Data);

	MonoField* FindFieldInClassByName(MonoClass* Class, std::string FieldName);

	DWORD_PTR GetStaticFieldAddress(MonoClass* Class, MonoField* Field);

	template <typename T>
	T GetStaticFieldValue(MonoClass* Class, MonoField* Field, std::string TypeName);

	MonoMethod* FindMethodInClass(MonoClass* Class, std::string MethodName, int ParaCnt = -1);

	std::string GetMethodSignature(MonoMethod* Method);

	DWORD_PTR CompileMethod(MonoMethod* Method);
};









template <typename T, typename... Args>
T MonoMethod::Call(Args... args)
{
	int CallType = CALL_TYPE_THISCALL;
	std::string ReturnTypeName = GetReturnTypeName();
	int ReturnType = FieldTypeNameMap.find(ReturnTypeName) != FieldTypeNameMap.end() ? FieldTypeNameMap[ReturnTypeName] : TYPE_VOID_P;

	if (Class->Instance == 0 and !IsStatic(this))
		printf("Not a static method, set class instance first\n");

	MonoNativeFunc* FunctObject = new MonoNativeFunc(Name);
	FunctObject->FunctionAddress = GetAddress();
	Class->ClassAPI->FunctSet->FunctPtrSet[Name] = FunctObject;
	mono_native_func_name.push_back(Name);
	mono_native_func_property[Name] = { ParamCnt, ReturnType };

	if (!IsStatic(this))
		return FunctObject->Call<T>(CALL_TYPE_THISCALL, *Class->ClassAPI->ThreadFunctionList, Class->Instance, args...);
	else
		return FunctObject->Call<T>(CALL_TYPE_THISCALL, *Class->ClassAPI->ThreadFunctionList, args...);
}



inline std::string GetReadType(std::string TypeName)
{
	if (FieldTypeNameMap.find(TypeName) != FieldTypeNameMap.end())
		return TypeName;
	else
		return "System.Pointer";
}


template <typename T>
inline T MonoClassAPI::GetStaticFieldValue(MonoClass* Class, MonoField* Field, std::string TypeName)
{
	DWORD_PTR Address = GetStaticFieldAddress(Class, Field);
	if (Address)
		return MonoUtils.ReadValue<T>(FieldTypeNameMap[GetReadType(TypeName)], Address);
	return T();
}

template <typename T>
inline T MonoField::GetValue()
{
	if (IsStatic(this))
		return Class->ClassAPI->GetStaticFieldValue<T>(Class, this, GetTypeName());
	else
		return MonoUtils.ReadValue<T>(FieldTypeNameMap[GetReadType(GetTypeName())], GetAddress());
}
