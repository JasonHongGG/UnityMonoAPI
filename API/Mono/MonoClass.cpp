#include "MonoClass.h"
#include "../Utils/Utils.h"

DWORD_PTR MonoMethod::GetAddress()
{
	if (!FunctionAdress)
		FunctionAdress = Class->ClassAPI->CompileMethod(this);
	return FunctionAdress;
}

std::string MonoMethod::GetSignature()
{
	if (Signature.empty())
		Signature = Class->ClassAPI->GetMethodSignature(this);
	return Signature;
}

std::string MonoMethod::GetReturnTypeName()
{
	std::string sig = GetSignature();
	size_t pos = sig.find(' ');
	if (pos != std::string::npos) {
		return sig.substr(0, pos);
	}
	return sig;
}

int MonoMethod::GetFlags()
{
	if (!Flags)
		Flags = Class->ClassAPI->FunctSet->FunctPtrSet["mono_method_get_flags"]->Call<int>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return Flags;
}

bool MonoMethod::IsStatic(MonoMethod* Method)
{
	if (!Method) return false;
	return (Method->GetFlags() & METHOD_ATTRIBUTE_STATIC) != 0;
}






int MonoField::GetFlags()
{
	if (!Flags)
		Flags = Class->ClassAPI->FunctSet->FunctPtrSet["mono_field_get_flags"]->Call<int>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return Flags;
}

DWORD_PTR MonoField::GetTypeAddress()
{
	if (!TypeAddress)
		TypeAddress = Class->ClassAPI->FunctSet->FunctPtrSet["mono_field_get_type"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return TypeAddress;
}

std::string MonoField::GetTypeName()
{
	if (TypeName.empty())
		TypeName = Class->ClassAPI->FunctSet->FunctPtrSet["mono_type_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, GetTypeAddress());
	
	return TypeName;
}

DWORD_PTR MonoField::GetAddress()
{
	if (IsStatic(this))
		Address = Class->ClassAPI->FunctSet->FunctPtrSet["get_static_field_address"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	else
		Address = GetInstance() + GetOffset();
	return Address;
}

DWORD_PTR MonoField::GetInstance()
{
	if (!Class->Instance) {
		printf("Current Class Has Not Instance");
		return 0x0;
	}
	return Class->Instance;
}

int MonoField::GetOffset()
{
	if (!Offset)
		Offset = Class->ClassAPI->FunctSet->FunctPtrSet["mono_field_get_offset"]->Call<int>(CALL_TYPE_CDECL, *Class->ClassAPI->ThreadFunctionList, Handle);
	return Offset;
}

bool MonoField::IsStatic(MonoField* Field)
{
	if (!Field) return false;
	return (Field->GetFlags() & (FIELD_ATTRIBUTE_STATIC | FIELD_ATTRIBUTE_HAS_FIELD_RVA)) != 0;
}








MonoField* MonoClass::FindField(std::string FieldName)
{
	MonoField* FieldObject =  ClassAPI->FindFieldInClassByName(this, FieldName);
	return FieldObject;
}

DWORD_PTR MonoClass::GetVtable()
{
	if (IsIL2CPP)
		return this->Handle;

	if (!VTable)
		VTable = ClassAPI->FunctSet->FunctPtrSet["mono_class_vtable"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ClassAPI->ThreadFunctionList, ClassAPI->ThreadFunctionList->at(0), this->Handle);
	return VTable;
}

MonoMethod* MonoClass::FindMethod(std::string MethodName, int ParaCnt)
{
	MonoMethod* MethodObject = ClassAPI->FindMethodInClass(this, MethodName, ParaCnt);
	return MethodObject;
}





MonoClass* MonoClassAPI::GetClassByImage(MonoImage* Image, std::string ClassName)
{
	// String Preprocess
	std::string ClassNamespace = "";
	size_t dot = ClassName.rfind('.');
	if (dot != std::string::npos) {
		ClassNamespace = ClassName.substr(0, dot);
		ClassName = ClassName.substr(dot + 1);
	}

	std::ranges::replace(ClassName, '+', '/'); // ±N '+' ´À´«¬° '/'

	// Get Class
	CString ClassNameObject(ClassName);
	CString ClassNamespaceObject(ClassNamespace);
	DWORD_PTR ClassAddress = 0x0;
	if (MonoNativeFuncSet::NativeFunctionExist(FunctSet, "mono_class_from_name_case"))
		ClassAddress = FunctSet->FunctPtrSet["mono_class_from_name_case"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Image->Handle, ClassNamespaceObject.Address, ClassNameObject.Address);
	if (!ClassAddress)
		ClassAddress = FunctSet->FunctPtrSet["mono_class_from_name"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Image->Handle, ClassNamespaceObject.Address, ClassNameObject.Address);
	ClassAddress &= 0xFFFFFFFFFFFF; // 12 bytes

	if (ClassAddress)
		return new MonoClass(this, Image, ClassAddress, ClassNameObject.Value, ClassNamespaceObject.Value);
	else
		return nullptr;
}

MonoClass* MonoClassAPI::FindClassInImageByName(std::string ImageName, std::string ClassName)
{
	MonoImage* Image = ImageAPI->FindImageByName(ImageName);
	if (Image)
		GetClassByImage(Image, ClassName);
	return nullptr;
}

std::vector<MonoClass*> MonoClassAPI::FindClassesInImageByName(std::string ImageName, std::vector<std::string> ClassNames)
{
	MonoImage* Image = ImageAPI->FindImageByName(ImageName);
	std::vector<MonoClass*> ResultClasses;
	if (Image)
		for (int i = 0; i < ClassNames.size(); i++) {
			std::string ClassName = ClassNames[i];
			ResultClasses.push_back(GetClassByImage(Image, ClassName));
		}
	return ResultClasses;
}

std::map<std::string, std::vector<MonoClass*>> MonoClassAPI::FindClassesInImageByNames(std::map<std::string, std::vector<std::string>> Data)
{
	std::map<std::string, std::vector<MonoClass*>> ResultClasses;
	std::vector<std::string > ImageNames;
	for (auto it = Data.begin(); it != Data.end(); ++it) {
		ImageNames.push_back(it->first);
	}
	std::vector<MonoImage*> ImageVector = ImageAPI->FindImagesByName(ImageNames);
	for (int i = 0; i < ImageVector.size(); i++) {
		MonoImage* CurImage = ImageVector[i];
		for (int j = 0; j < Data[CurImage->Name].size(); j++) {
			ResultClasses[CurImage->Name].push_back(GetClassByImage(CurImage, Data[CurImage->Name][j]));
		}
	}
	return ResultClasses;


}

MonoField* MonoClassAPI::FindFieldInClassByName(MonoClass* Class, std::string FieldName)
{
	CString FieldNameObject(FieldName);
	DWORD_PTR FieldAddress = FunctSet->FunctPtrSet["mono_class_get_field_from_name"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Class->Handle, FieldNameObject.Address);
	MonoField* FieldObj = new MonoField(Class, FieldAddress, FieldName);
	return FieldObj;
}

DWORD_PTR MonoClassAPI::GetStaticFieldAddress(MonoClass* Class, MonoField* Field)
{
	if (IsIL2CPP) return 0x0;
	DWORD_PTR vtable = Class->GetVtable();
	if (vtable) {
		DWORD_PTR StaticFieldData = FunctSet->FunctPtrSet["mono_vtable_get_static_field_data"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, vtable);
		if (StaticFieldData) {
			int Offset = FunctSet->FunctPtrSet["mono_field_get_offset"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Field->Handle);
			return StaticFieldData + Offset;
		}
	}
	return 0x0;
}

MonoMethod* MonoClassAPI::FindMethodInClass(MonoClass* Class, std::string MethodName, int ParaCnt)
{
	CString MethodNameObject(MethodName);
	DWORD_PTR MethodAddress = FunctSet->FunctPtrSet["mono_class_get_method_from_name"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Class->Handle, MethodNameObject.Address, ParaCnt);
	MethodAddress &= 0xFFFFFFFFFFFF;
	return new MonoMethod(Class, MethodAddress, MethodName);
}

std::string MonoClassAPI::GetMethodSignature(MonoMethod* Method)
{
	if (IsIL2CPP) {
		int ParamCnt = FunctSet->FunctPtrSet["il2cpp_method_get_param_count"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle);
		std::string ParaInfo = "";
		for (int i = 0; i < ParamCnt; i++) {
			std::string ParamName = FunctSet->FunctPtrSet["il2cpp_method_get_param_name"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle, i);
			DWORD_PTR ParamTypeAddress = FunctSet->FunctPtrSet["il2cpp_method_get_param"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle, i);
			std::string TypeName = FunctSet->FunctPtrSet["mono_type_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, ParamTypeAddress); //TODO
			ParaInfo += TypeName + " " + ParamName + ", ";
		}
		DWORD_PTR ReturnTypeAddress = FunctSet->FunctPtrSet["il2cpp_method_get_return_type"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle);
		std::string ReturnTypeName = FunctSet->FunctPtrSet["mono_type_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, ReturnTypeAddress); //TODO
		return ReturnTypeName + " (" + ParaInfo.substr(0, ParaInfo.size() - 2) + ")";	// Remove ", " at the end
	}
	else {
		DWORD_PTR MethodSigAddress = FunctSet->FunctPtrSet["mono_method_signature"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle);
		std::string ParamDesc = FunctSet->FunctPtrSet["mono_signature_get_desc"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, MethodSigAddress, 1); // Param Type
		int ParamCnt = FunctSet->FunctPtrSet["mono_signature_get_param_count"]->Call<int>(CALL_TYPE_CDECL, *ThreadFunctionList, MethodSigAddress);
		Method->ParamCnt = ParamCnt;

		// Get Param Names
		std::vector<std::string> ParamNames;
		if (ParamCnt)
		{
			std::vector<DWORD_PTR> temp(ParamCnt, 0);
			CArray NamesPtrArray(temp);
			FunctSet->FunctPtrSet["mono_method_get_param_names"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle, NamesPtrArray.Address);
			NamesPtrArray.ReadResult();
			for (int i = 0; i < ParamCnt; i++) {
				BYTE ParamNameBytes[60];
				MemMgr.MemReader.ReadString(NamesPtrArray.Elements[i], ParamNameBytes);
				std::string ParamName(reinterpret_cast<char*>(ParamNameBytes));
				ParamNames.push_back(ParamName);
			}

		}

		// Get Return Type
		DWORD_PTR ReturnTypeAddress = FunctSet->FunctPtrSet["mono_signature_get_return_type"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, MethodSigAddress);
		std::string	ReturnType = FunctSet->FunctPtrSet["mono_type_get_name"]->Call<std::string>(CALL_TYPE_CDECL, *ThreadFunctionList, ReturnTypeAddress);

		// Get Param Info
		std::string ParaInfo = "";
		if (ParamCnt > 0 || !ParamDesc.empty()) {
			std::vector<std::string> ParamTypeVector = Utils.GetTokens(ParamDesc, ',');

			if (ParamCnt == static_cast<int>(ParamTypeVector.size())) {
				std::ostringstream oss;
				for (int i = 0; i < ParamCnt; ++i) {
					if (i > 0) oss << ", ";
					oss << ParamTypeVector[i] << " " << ParamNames[i];
				}
				ParaInfo = oss.str();
			}
			else {
				ParaInfo = "<parse error>";
			}
		}

		return ReturnType + " (" + ParaInfo + ")";
	}
}

DWORD_PTR MonoClassAPI::CompileMethod(MonoMethod* Method)
{
	if (IsIL2CPP) {
		DWORD_PTR Address;
		MemMgr.MemReader.ReadMem<DWORD_PTR>(Address, Method->Handle);
		return Address;
	}
	else {
		DWORD_PTR ClassAddress = FunctSet->FunctPtrSet["mono_method_get_class"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle);
		if (!ClassAddress) return 0;

		bool IsGenericExist = FunctSet->FunctPtrSet.find("mono_class_is_generic") != FunctSet->FunctPtrSet.end() and FunctSet->FunctPtrSet["mono_class_is_generic"]->FunctionAddress != 0;
		if (
			(
				IsGenericExist and
				FunctSet->FunctPtrSet["mono_class_is_generic"]->Call<int>(CALL_TYPE_CDECL, *ThreadFunctionList, ClassAddress) == 0
				) or
			!IsGenericExist
			) {
			return FunctSet->FunctPtrSet["mono_compile_method"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, *ThreadFunctionList, Method->Handle);
		}
	}
	return 0;
}