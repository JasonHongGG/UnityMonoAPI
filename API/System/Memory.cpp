
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
    // �N DWORD_PTR ���ഫ���r�`�Ʋ�
    for (size_t i = 0; i < sizeof(DWORD_PTR); ++i) {
        BytesArray[i] = static_cast<BYTE>((Address >> (i * 8)) & 0xFF);
    }
}

std::vector<BYTE> Memory::HexStringToBytes(const std::string& hex) {
    //�}�X�x�s���G���Ŷ�
    std::vector<BYTE> bytes;
    bytes.reserve(hex.length() / 2);    //�w�d�Ŷ��A�קK���_ reallocate

    //���Φr��A�q���@�Ӥ@�� token�A�]�N�O�@�Ӥ@�� bytes
    std::vector<std::string> tokens = Utils.GetTokens(hex);;

    //byte�r��̧��ഫ�� 16 �i��
    for (const auto& token : tokens) {
        if (token != "?" && token != "??") {            //�p�G token �O 3F �]�L�ҿסA�ᤩ '?' �]�O�@�˷|�� 3F�A�o��]�O���}�o�̤�K�b�@�}�l���r��аO���� byte �� ??
            unsigned int value;
            std::istringstream converter(token);
            converter >> std::hex >> value;
            bytes.push_back(static_cast<BYTE>(value));
        }
        else bytes.push_back(static_cast<BYTE>('?'));
    }

    // �˵��ഫ�᪺���G
    std::cout << "[Converted bytes]" << std::endl;
    for (const auto& byte : bytes) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    return bytes;
}





