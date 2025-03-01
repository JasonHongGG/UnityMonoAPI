#pragma once
#include "../Utils/UtilsConst.h"

inline bool IsIL2CPP = false;

inline std::map<std::string, ValueTypeEnum> FieldTypeNameMap = {
    {"System.Boolean", TYPE_BOOL},
    {"System.SByte", TYPE_CHAR},
    {"System.Byte", TYPE_UCHAR},
    {"System.Int16", TYPE_INT16},
    {"System.UInt16", TYPE_UINT16},
    {"System.Int32", TYPE_INT32},
    {"System.UInt32", TYPE_UINT32},
    {"System.Int64", TYPE_INT64},
    {"System.UInt64", TYPE_UINT64},
    {"System.Single", TYPE_FLOAT},
    {"System.Double", TYPE_DOUBLE},
    {"System.String", TYPE_CHAR_P},
    {"System.Decimal", TYPE_VOID_P},
    {"System.Pointer", TYPE_VOID_P},
};