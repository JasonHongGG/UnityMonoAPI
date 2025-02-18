### Unity Mono Testing Tool

---

提供一個簡易的 Mono API 讓開發者獨立於開發環境外，方面的作 Unit Testing，測試 Release Version 的功能

---

以下提供一個簡單的範例，範例中的參數名稱為開發者自行定義的  
開發者在 Unity 中定義的 Class 都可以在 Assembly-CSharp.dll 中定位到  
Ex: 開發者定義 PlayerHandler，則該 PlayerHandler 預設會在 Assembly-CSharp.dll 底下

```
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
	// MonoClass 定義格式 為 {DLL Name, { 在該 DLL 底下定義的 Class }}
    {"Assembly-CSharp", {"PlayerHandler", "Player"}},
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

		CurrentRound++;
	}
}
```

---

### 未來規劃

支援 Window 32 bit 平台
支援 Android 平台
支援 IL2CPP
