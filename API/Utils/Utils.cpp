#pragma once
#include "Utils.h"

std::string UtilsSet::ImplicitStringWrapper(const char* format, va_list args) {
    static char Msg[128] = "";
    vsnprintf(Msg, sizeof(Msg), format, args);
    return std::string(Msg);
}

std::string UtilsSet::StringWrapper(const char* format, ...)
{
    if (format) {
        va_list args;
        va_start(args, format);
        std::string result = ImplicitStringWrapper(format, args);
        va_end(args);
        return result;
    }
    return "";
}

// Normal Tool
void UtilsSet::CopyToClipBoard(const std::string& Str)
{
    const std::wstring UnicodeStr = ANSIToUnicode(Str.c_str());

    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();

    const auto size = (UnicodeStr.size() + 1) * sizeof(wchar_t);
    const auto hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hGlobal)
    {
        CloseClipboard();
        return;
    }

    const auto lpGlobal = GlobalLock(hGlobal);
    if (!lpGlobal)
    {
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    memcpy(lpGlobal, UnicodeStr.c_str(), size);
    GlobalUnlock(hGlobal);

    SetClipboardData(CF_UNICODETEXT, hGlobal); //CF_UNICODETEXT  CF_TEXT
    CloseClipboard();
}


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

int UtilsSet::FindRightMostSetBit(int num) {
    int position = 0;
    while (num) {
        if (num & 1) return position;

        num >>= 1;  // shift 1 bit
        ++position;
    }
    return -1;
}


// String
bool UtilsSet::ContainStr(std::string& Str, std::string TargetStr)
{
    if (Str.find(TargetStr) != std::string::npos) return true;
    else return false;
}

bool UtilsSet::IsHex(const std::string& str) {
    if (str.empty()) return false;

    for (char c : str) {
        if (!std::isxdigit(c)) { // 浪琩琌せ秈计
            return false;
        }
    }
    return true;
}

std::vector<std::string> UtilsSet::GetTokens(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string UtilsSet::FindSubStr(std::string input) {
    // ﹚竡璶で皌﹃ EX : std::string input = "mov rax, qword [rsp+0x30]";

    std::regex pattern("\\[([^\\]]+)\\]");

    // 秈︽で皌
    std::smatch matches;
    if (std::regex_search(input, matches, pattern)) {
        std::string match = matches[1].str();
        std::cout << "で皌场だ: " << match << std::endl;
        return match;
    }
    else {
        std::cout << "⊿Τтで皌场だ" << std::endl;
        return "";
    }
}

std::string UtilsSet::HexToString(size_t Hex) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << Hex; // 锣传Θ16秈计﹃
    std::string HexStr = ss.str();
    return HexStr;
}

size_t UtilsSet::StringToHex(std::string HexStr) {
    if (HexStr.find("0x") != std::string::npos) HexStr.erase(HexStr.find("0x"), 2);
    if (HexStr.find("0X") != std::string::npos) HexStr.erase(HexStr.find("0X"), 2);

    return std::stoi(HexStr, nullptr, 16);
}

void UtilsSet::StringToLower(std::string& Str) {
    for (char& c : Str) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

std::string UtilsSet::rStringToLower(std::string Str) {
    for (char& c : Str) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return Str;
}

bool UtilsSet::IsStringContainControlChar(const std::string Str) {
    for (size_t i = 0; i < Str.length(); ++i) {
        if ((Str[i] >= 0x00 && Str[i] <= 0x1F) || Str[i] == 0x7F) { // 狦琌北才
            //std::cout << "т北才 '" << Str[i] << "'竚琌" << i << std::endl;
            return true;
        }
    }
    return false;
}

int UtilsSet::BytesToNum(const std::vector<BYTE> Data) {
    int Result = 0;
    for (size_t i = 0; i < Data.size(); ++i) {
        Result |= static_cast<int>(Data[i]) << (i * 8);
    }
    return Result;
};


// Transform String Type
// ansi 虫 bytesunicode  bytes ボ┮Τ才 UTF8  unicode 贺絪絏よΑ
std::string UtilsSet::UnicodeToUTF8(const wchar_t* UnicodeString) {
    // 璸衡 UTF8 ┮惠﹃锣传
    int utf8_length = WideCharToMultiByte(CP_UTF8, 0, UnicodeString, -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(utf8_length, 0);
    WideCharToMultiByte(CP_UTF8, 0, UnicodeString, -1, &utf8_str[0], utf8_length, nullptr, nullptr);

    return utf8_str;
}

std::wstring UtilsSet::UTF8ToUnicode(const char* utf8String) {
    int unicodeLength = MultiByteToWideChar(CP_ACP, 0, utf8String, -1, nullptr, 0);
    std::wstring unicodeString(unicodeLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, utf8String, -1, &unicodeString[0], unicodeLength);
    return unicodeString;
}

std::string UtilsSet::UnicodeToANSI(const wchar_t* UnicodeString) {
    // 璸衡 ANSI ┮惠﹃锣传
    int ansi_length = WideCharToMultiByte(CP_ACP, 0, UnicodeString, -1, NULL, 0, NULL, NULL);
    std::string ansi_str(ansi_length, 0);
    WideCharToMultiByte(CP_ACP, 0, UnicodeString, -1, &ansi_str[0], ansi_length, NULL, NULL);

    return ansi_str;
}

std::wstring UtilsSet::ANSIToUnicode(const char* ansiString) {
    int unicodeLength = MultiByteToWideChar(CP_ACP, 0, ansiString, -1, nullptr, 0);
    std::wstring unicodeString(unicodeLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString, -1, &unicodeString[0], unicodeLength);
    return unicodeString;
}

std::string UtilsSet::ANSIToUTF8(const char* ansiString) {
    return UnicodeToUTF8(ANSIToUnicode(ansiString).c_str());
}

std::string UtilsSet::UTF8ToANSI(const char* utf8String) {
    return UnicodeToUTF8(ANSIToUnicode(utf8String).c_str());
}


// Time
std::string UtilsSet::GetCurTime()
{
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y/%m/%d %H:%M:%S");
    return oss.str();
}

std::string UtilsSet::GetTime(bool GetCurrentTime, std::chrono::steady_clock::time_point& StartTime, std::chrono::steady_clock::time_point& EndTime, std::chrono::steady_clock::time_point& CurTime)
{
    std::chrono::duration<double> Duration;
    if (GetCurrentTime) {	// Current Time
        CurTime = std::chrono::high_resolution_clock::now();
        Duration = CurTime - StartTime;
    }
    else {					// Last Record Time
        Duration = EndTime - StartTime;
    }

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(Duration);
    Duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(Duration);
    Duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(Duration);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes.count() << ":"
        << std::setw(2) << std::setfill('0') << seconds.count() << ":"
        << std::setw(3) << std::setfill('0') << milliseconds.count();

    return oss.str();
}

void UtilsSet::TimerSwitch(bool Activate, bool* TimerActivate, std::chrono::steady_clock::time_point& StartTime, std::chrono::steady_clock::time_point& EndTime)
{
    if (Activate) {
        StartTime = std::chrono::high_resolution_clock::now();
        *(TimerActivate) = true;
    }
    else {
        EndTime = std::chrono::high_resolution_clock::now();
        *(TimerActivate) = false;
    }
}