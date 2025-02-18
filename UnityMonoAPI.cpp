// UnityMonoAPI.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include "API/MonoAPI.h"


int main()
{
	InitialProcess(L"Content Warning.exe");

	// ==========  GetPlayerInfo  ==========
	DWORD_PTR CurrentPlagerInstance = 0x0;
	MonoClass* PlayerClass;
	MonoMethod* HealMethod;
	MonoMethod* TakeDamageMethod;
	std::map<std::string, std::vector<MonoClass*>> MonoClassMap = MonoMgr.ClassAPI->FindClassesInImageByNames({
		{"Assembly-CSharp", {"BotHandler", "PlayerHandler", "Player"}},
		{"UnityEngine.CoreModule", {"UnityEngine.Component", "UnityEngine.Transform", "UnityEngine.Camera"}}
		});

	MonoClass* PlayerHandlerClass = MonoClassMap["Assembly-CSharp"][1];
	PlayerHandlerClass->Instance = PlayerHandlerClass->FindField("instance")->GetValue<DWORD_PTR>();
	DWORD_PTR PlayerListInstance = PlayerHandlerClass->FindField("players")->GetValue<DWORD_PTR>();
	DWORD_PTR PlayerListBaseAddress = 0x0;
	MemMgr.MemReader.ReadMem<DWORD_PTR>(PlayerListBaseAddress, PlayerListInstance + 0x10);
	int PlayerListSize = 0;
	MemMgr.MemReader.ReadMem<int>(PlayerListSize, PlayerListInstance + 0x18);
	std::vector<DWORD_PTR> PlayerList = MemMgr.MemReader.ReadArray<DWORD_PTR>(PlayerListBaseAddress + 0x20, PlayerListSize);
	CurrentPlagerInstance = PlayerList[0];

	PlayerClass = MonoClassMap["Assembly-CSharp"][2];
	HealMethod = PlayerClass->FindMethod("CallHeal");
	TakeDamageMethod = PlayerClass->FindMethod("TakeDamage");
	printf("Complete 0x%llx\n", CurrentPlagerInstance);


	// ==========  Test Area  ==========

	if (CurrentPlagerInstance) {
		PlayerClass->Instance = CurrentPlagerInstance;
		while (true) {
			printf("Take Damage ID 1");
			TakeDamageMethod->Call<float>((float)10);
			Sleep(1000);
			printf("Take Damage ID 2");
			TakeDamageMethod->Call<float>((float)10);
			Sleep(1000);
			printf("Take Damage ID 3");
			TakeDamageMethod->Call<float>((float)10);
			Sleep(1000);
			printf("Heal ID 1");
			HealMethod->Call<float>((float)20);
			Sleep(1000);
			printf("Heal ID 2");
			HealMethod->Call<float>((float)20);
			Sleep(1000);
		}

	}
	else
	{
		printf("Player Not Found !!!");
		exit(0);
	}
}