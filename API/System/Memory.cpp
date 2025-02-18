
#pragma once
#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>

#include "../Utils/Include/BS_thread_pool.hpp"
#include "Memory.h"

void Memory::AddressToBytes(DWORD_PTR Address, BYTE BytesArray[])
{
    // 將 DWORD_PTR 值轉換成字節數組
    for (size_t i = 0; i < sizeof(DWORD_PTR); ++i) {
        BytesArray[i] = static_cast<BYTE>((Address >> (i * 8)) & 0xFF);
    }
}

std::vector<BYTE> Memory::HexStringToBytes(const std::string& hex) {
    //開出儲存結果的空間
    std::vector<BYTE> bytes;
    bytes.reserve(hex.length() / 2);    //預留空間，避免不斷 reallocate

    //分割字串，猜成一個一個 token，也就是一個一個 bytes
    std::vector<std::string> tokens = Utils.GetTokens(hex);;

    //byte字串依序轉換成 16 進位
    for (const auto& token : tokens) {
        if (token != "?" && token != "??") {            //如果 token 是 3F 也無所謂，賦予 '?' 也是一樣會給 3F，這邊也是讓開發者方便在一開始的字串標記未知 byte 為 ??
            unsigned int value;
            std::istringstream converter(token);
            converter >> std::hex >> value;
            bytes.push_back(static_cast<BYTE>(value));
        }
        else bytes.push_back(static_cast<BYTE>('?'));
    }

    // 檢視轉換後的結果
    std::cout << "[Converted bytes]" << std::endl;
    for (const auto& byte : bytes) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    return bytes;
}





