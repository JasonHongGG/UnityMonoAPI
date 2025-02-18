#pragma once
#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <cstdarg>
#include <chrono>
#include <Windows.h>

class UtilsSet
{
public:

    std::string ImplicitStringWrapper(const char* format, va_list args);

    std::string StringWrapper(const char* format, ...);

    // Normal Tool
    void CopyToClipBoard(const std::string& Str);


    // Bit Operation
    const std::map<int, uint8_t> BitMaskTable = {
        { 0 , 0x01 },
        { 1 , 0x02 },
        { 2 , 0x04 },
        { 3 , 0x08 },
        { 4 , 0x10 },
        { 5 , 0x20 },
        { 6 , 0x40 },
        { 7 , 0x80 }
    };

    int FindRightMostSetBit(int num);


    // String
    bool ContainStr(std::string& Str, std::string TargetStr);

    bool IsHex(const std::string& str);

    std::vector<std::string> GetTokens(const std::string& input, char delimiter = ' ');

    std::string FindSubStr(std::string input);

    std::string HexToString(size_t Hex);

    size_t StringToHex(std::string HexStr);

    void StringToLower(std::string& Str);

    std::string rStringToLower(std::string Str);

    bool IsStringContainControlChar(const std::string Str);

    // number
    int BytesToNum(const std::vector<BYTE> Data);


    // Transform String Type
    // ansi 為單 bytes，unicode 為多 bytes 可表示所有字符， UTF8 為 unicode 的一種編碼方式
    std::string UnicodeToUTF8(const wchar_t* UnicodeString);

    std::wstring UTF8ToUnicode(const char* utf8String);

    std::string UnicodeToANSI(const wchar_t* UnicodeString);

    std::wstring ANSIToUnicode(const char* ansiString);

    std::string ANSIToUTF8(const char* ansiString);

    std::string UTF8ToANSI(const char* utf8String);


    // Time
    std::string GetCurTime();

    std::string GetTime(bool GetCurrentTime, std::chrono::steady_clock::time_point& StartTime, std::chrono::steady_clock::time_point& EndTime, std::chrono::steady_clock::time_point& CurTime);

    void TimerSwitch(bool Activate, bool* TimerActivate, std::chrono::steady_clock::time_point& StartTime, std::chrono::steady_clock::time_point& EndTime);

private:

};

inline UtilsSet Utils = UtilsSet();