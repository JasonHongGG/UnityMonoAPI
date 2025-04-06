// UnityMonoAPI.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include "API/MonoAPI.h"

void Test1()
{
	// 初始化 Process，參數為 Process Name
	InitialProcess(L"Content Warning.exe");

	// 參數定義
	DWORD_PTR CurrentPlagerInstance = 0x0;
	MonoClass* PlayerClass;
	MonoMethod* HealMethod;
	MonoMethod* TakeDamageMethod;
	DWORD_PTR PlayerListBaseAddress = 0x0;
	int PlayerListSize = 0;
	std::map<std::string, std::vector<MonoClass*>> MonoClassMap = MonoMgr.ClassAPI->FindClassesInImageByNames({
		{"Assembly-CSharp", {"BotHandler", "PlayerHandler", "Player"}},
		});

	// 取得 PlayerListInstance
	MonoClass* PlayerHandlerClass = MonoClassMap["Assembly-CSharp"][1];
	PlayerHandlerClass->Instance = PlayerHandlerClass->FindField("instance")->GetValue<DWORD_PTR>();
	DWORD_PTR PlayerListInstance = PlayerHandlerClass->FindField("players")->GetValue<DWORD_PTR>();

	// Unity 的 List 類別， 偏移量 0x10 為 _items、偏移量 0x18 為 _size
	MemMgr.MemReader.ReadMem<DWORD_PTR>(PlayerListBaseAddress, PlayerListInstance + 0x10);
	MemMgr.MemReader.ReadMem<int>(PlayerListSize, PlayerListInstance + 0x18);
	// _items 偏移量 0x20 為第一個 item 的地址
	std::vector<DWORD_PTR> PlayerList = MemMgr.MemReader.ReadArray<DWORD_PTR>(PlayerListBaseAddress + 0x20, PlayerListSize);
	CurrentPlagerInstance = PlayerList[0];

	// 根據 Player Class 取出 CallHeal、TakeDamage 的 Fucntion
	PlayerClass = MonoClassMap["Assembly-CSharp"][2];
	HealMethod = PlayerClass->FindMethod("CallHeal");
	TakeDamageMethod = PlayerClass->FindMethod("TakeDamage");

	// ====================  Test Area  ====================
	// 此測試項目是測試 CallHeal、TakeDamage 的功能、動畫是否正常
	int TestRoundCnt = 5;
	int CurrentRound = 0;
	if (CurrentPlagerInstance) {
		PlayerClass->Instance = CurrentPlagerInstance;
		while (CurrentRound <= TestRoundCnt) {
			printf("Take Damage ID 1\n");
			TakeDamageMethod->Call<float>((float)10);
			Sleep(1000);
			printf("Take Damage ID 2\n");
			TakeDamageMethod->Call<float>((float)10);
			Sleep(1000);
			printf("Take Damage ID 3\n");
			TakeDamageMethod->Call<float>((float)10);
			Sleep(1000);
			printf("Heal ID 1\n");
			HealMethod->Call<float>((float)20);
			Sleep(1000);
			printf("Heal ID 2\n");
			HealMethod->Call<float>((float)20);
			Sleep(1000);
			CurrentRound++;
		}
	}
}

void Test2()
{
	InitialProcess(L"Wildfrost.exe");
	std::map<std::string, std::vector<MonoClass*>> MonoClassMap = MonoMgr.ClassAPI->FindClassesInImageByNames({
		{"Assembly-CSharp.dll", {"ShopRoutine"}},
		});
	MonoClass* ShopRoutine = MonoClassMap["Assembly-CSharp.dll"][0];
	ShopRoutine->Instance = 0x000001F95D39CD00;
	MonoMethod* SetUp = ShopRoutine->FindMethod("Close");
	SetUp->Call<DWORD_PTR>();
}

void Test3()
{
	// Load Custom DLL 
	InitialProcess(L"REPO.exe");
	std::string assemblyPath = "r.e.p.o.dll";
	std::vector<uint8_t> assembly = MemMgr.MemReader.ReadAllBytesOfFile(assemblyPath);
	DWORD_PTR rawImageAddress = MonoMgr.ImageAPI->OpenRawImageFromData(assembly);
	DWORD_PTR assemblyAddress = MonoMgr.ImageAPI->OpenAssemblyFromRawImage(rawImageAddress);

	std::map<std::string, std::vector<MonoClass*>> MonoClassMap = MonoMgr.ClassAPI->FindClassesInImageByNames({
		{"r.e.p.o cheat", {"Loader"}},
		});

	MonoClass* Loader = MonoClassMap["r.e.p.o cheat"][0];
	MonoMethod* Init = Loader->FindMethod("Init");
	Init->Call<DWORD_PTR>();
}

int main()
{
	Test3();
}