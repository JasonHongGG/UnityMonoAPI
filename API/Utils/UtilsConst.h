#pragma once

inline int CALL_TYPE_DEF = 0;
inline int CALL_TYPE_CDECL = 1;
inline int CALL_TYPE_STDCALL = 2;
inline int CALL_TYPE_THISCALL = 3;
inline int CALL_TYPE_FASTCALL = 4;

enum ValueTypeEnum {
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_UCHAR,
    TYPE_INT16,
    TYPE_UINT16,
    TYPE_INT32,
    TYPE_UINT32,
    TYPE_INT64,
    TYPE_UINT64,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_VOID_P,
    TYPE_CHAR_P,
};