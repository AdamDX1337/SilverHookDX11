#include "includes.h"
#include "sdk.h"
#include "kiero/minhook/include/MinHook.h"
#include <vector>
#include <Windows.h>
#include <iostream>
#include <string>

//Full Credits
// "batman" (Was credited in MastoidHook)
// Mio (original miohook)
// Mastoid (MastoidHook)
// SilverXK (Updated Hook) 
// JewishTricks (Help with Cheats)
// Chocolatte (Finding and testing DX11 Hook)
// Kiero (For the DX11 Hook)

// Removed Features for public
// DLC Enabler/Disabler
// Freeze Host
// DOS Host
// File Transer
// All the memory addresses



extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;



typedef bool(__fastcall* CSessionPost)(void* pThis, CCommand* pCommand, bool ForceSend);
CSessionPost CSessionPostHook;
CSessionPost CSessionPostTramp;

typedef CAddPlayerCommand* (__fastcall* GetCAddPlayerCommand)(void* pThis, CString* User, CString* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7);
GetCAddPlayerCommand CAddPlayerCommandHook;
GetCAddPlayerCommand CAddPlayerCommandTramp;

typedef CStartGameCommand* (__fastcall* GetCStartGameCommand)(void* pThis);
GetCStartGameCommand CStartGameCommandFunc;

typedef LPVOID(__fastcall* GetCCommand)(__int64 a1);
GetCCommand GetCCommandFunc;

void* pCSession = nullptr;
void* pCAddPlayer = nullptr;
int KeyArray[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };
char TagBuffer[8];
bool bDebugOutputEnabled = true;
bool bMaxNameSize = false;
bool bJoinAsGhost = false;
bool bFakePname = false;
bool bEnabletdebug = false;
bool bMenuOpen = true;
bool bCE = false;
int iMyMachineID;
int iMachineIDFake = 50;
__int64 iParadoxSocialID = 0;
int dUnknown;
DWORD* dT;
int dM;
bool lala = false;
int fM;
__int64 dP;
CString* dN;
CString* dNN;
__int64 dReason;
CString* empty = new CString;

uintptr_t GameBase = (uintptr_t)GetModuleHandleA("hoi4.exe");

void PatchMemory(uintptr_t address, unsigned char* patch, DWORD size)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((LPVOID)address, patch, size);
	VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
}


void MultiplayerLobbyHack()
{

	uintptr_t address = NULL /*address*/;

	unsigned char patch[] = { NULL /*bytes*/};

	void* newmem = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (newmem != nullptr)
	{
		DWORD relativeOffset = (NULL /*address*/) - (address + sizeof(patch));
		memcpy(newmem, patch, sizeof(patch));
		*(BYTE*)((uintptr_t)newmem) = NULL /*bytes*/;
		*(BYTE*)((uintptr_t)newmem + 1) = NULL /*bytes*/;
		*(DWORD*)((uintptr_t)newmem + 2) = relativeOffset;
		PatchMemory(address, (unsigned char*)newmem, sizeof(patch));
	}
}

template <typename T>
T ReadMemory(uintptr_t address)
{
	return *reinterpret_cast<T*>(address);
}

uintptr_t OffsetCalculator(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets)
{
	uintptr_t address = baseAddress;
	for (uintptr_t offset : offsets)
	{
		address = ReadMemory<uintptr_t>(address);
		address += offset;
	}
	return address;
}










void ChangeIntAddressValue(uintptr_t bAddr, uintptr_t bOff, int Tag)
{

	uintptr_t baseAddress = bAddr; 
	
	std::vector<uintptr_t> offsets = { bOff };
	
	uintptr_t finalAddress = OffsetCalculator(GameBase + baseAddress, offsets);

	DWORD* pValue = reinterpret_cast<DWORD*>(finalAddress);
	DWORD oldProtect;

	(VirtualProtect(pValue, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect));

	*pValue = Tag;

	VirtualProtect(pValue, sizeof(DWORD), oldProtect, &oldProtect);

}

void ChangeByteAddressValue(uintptr_t addr)
{
	uintptr_t address = GameBase + addr; 
	BYTE* pValue = (BYTE*)address;
	DWORD oldProtect;


	VirtualProtect(pValue, sizeof(BYTE), PAGE_EXECUTE_READWRITE, &oldProtect);
	
	if (*pValue == 1)
	{
		*pValue = 0;
	}
	else
	{
		*pValue = 1;
	}
	
	VirtualProtect(pValue, sizeof(BYTE), oldProtect, &oldProtect);
}











void InitImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	if (GetAsyncKeyState(VK_INSERT) & 1)
		bMenuOpen = !bMenuOpen;

	if (bMenuOpen)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureMouse = true;
		io.WantCaptureKeyboard = true;

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
		}
		else
		{
			io.MouseReleased[0] = true;
			io.MouseDown[0] = false;
			io.MouseClicked[0] = false;
		}

		for (int i : KeyArray)
		{
			if (GetAsyncKeyState(i) & 1)
			{
				io.AddInputCharacter(i);
			}
		}





		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		ImGui::Begin("SilverHook DX11", &bMenuOpen);
		ImGui::Text("Cheat Settings");
		ImGui::Checkbox("Spoof Steam Name", &bMaxNameSize);
		ImGui::Checkbox("Join As Ghost", &bJoinAsGhost);
		ImGui::Checkbox("Multiplayer Lobby Hack", &bCE);
		if (bCE) {
			MultiplayerLobbyHack();
		}
		ImGui::Text("");
		ImGui::Text("Function Calls");
		ImGui::Columns(2);
		if (ImGui::Button("Add Fake Player", ImVec2(140, 28)) && pCSession != nullptr)
		{
			DWORD* tt = dT;
			int tM = dM;
			__int64 tP = dP;
			CString* tN = dN;
			CString* tNN = dNN;
			CString* empty = new CString;

			CAddPlayerCommand* AddFake = (CAddPlayerCommand*)GetCCommandFunc(168);
			fM = 20;
			fM++;
			dM++;

			AddFake = CAddPlayerCommandTramp(AddFake, tN, tNN, tt, dM, false, tP);

			CSessionPostTramp(pCSession, AddFake, true);
		}
		ImGui::NextColumn();
		if (ImGui::Button("Fake Kick", ImVec2(140, 28)) && pCSession != nullptr)
		{
			DWORD* tt = dT;
			int tM = dM;
			__int64 tP = dP;
			CString* tN = dN;
			CString* tNN = dNN;

			CAddPlayerCommand* FakeKick = (CAddPlayerCommand*)GetCCommandFunc(168);
			FakeKick = CAddPlayerCommandTramp(FakeKick, tN, tNN, tt, tM, false, tP);
			CSessionPostTramp(pCSession, FakeKick, true);
		}
		ImGui::NextColumn();
		if (ImGui::Button("Start Game", ImVec2(140, 28)) && pCSession != nullptr)
		{
			CStartGameCommand* StartGame = (CStartGameCommand*)GetCCommandFunc(41);
			StartGame = CStartGameCommandFunc(StartGame);
			CSessionPostTramp(pCSession, StartGame, true);
		}
		ImGui::Columns(1);
		ImGui::Text("Mem Addresses");
		ImGui::Columns(2);
		if (ImGui::Button("FOW", ImVec2(140, 28)) && pCSession != nullptr)
		{
			ChangeByteAddressValue(NULL /*address*/);
		}
		ImGui::NextColumn();
		if (ImGui::Button("AllowTraits", ImVec2(140, 28)) && pCSession != nullptr)
		{
			ChangeByteAddressValue(NULL /*address*/);
		}
		ImGui::NextColumn();
		if (ImGui::Button("Debug", ImVec2(140, 28)) && pCSession != nullptr)
		{
			ChangeByteAddressValue(NULL /*address*/);
		}
		ImGui::NextColumn();
		if (ImGui::Button("Human AI", ImVec2(140, 28)) && pCSession != nullptr)
		{
			ChangeByteAddressValue(NULL /*address*/);
		}ImGui::NextColumn();
		if (ImGui::Button("AI", ImVec2(140, 28)) && pCSession != nullptr)
		{
			ChangeByteAddressValue(NULL /*address*/);
		}
		ImGui::Columns(1);
		ImGui::Text("Tag Switching");
		ImGui::SetNextItemWidth(70.f);
		ImGui::InputText("", TagBuffer, IM_ARRAYSIZE(TagBuffer));
		ImGui::SameLine();
		if (ImGui::Button("Tag Switch") && pCSession != nullptr)
		{
			std::string sTagBuffer = TagBuffer;

			if (sTagBuffer.length() > 0)
			{
				int iNewTag = std::stoi(sTagBuffer);


				ChangeIntAddressValue(NULL /*address*/, NULL /*offset*/, iNewTag);
			}
		}
		ImGui::Text("");
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			memset(TagBuffer, 0, sizeof(TagBuffer));
		}
		ImGui::Text("");
		ImGui::Text("Credits: SilverXK & Chocolatte");
		ImGui::End();
	
		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}

bool __fastcall hkCSessionPost(void* pThis, CCommand* pCommand, bool ForceSend)
{
	pCSession = pThis;
	return CSessionPostTramp(pThis, pCommand, ForceSend);
}

CAddPlayerCommand* __fastcall hkCAddPlayerCommand(void* pThis, CString* User, CString* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7)
{
	if (iParadoxSocialID == 0)
		iParadoxSocialID = a7;

	iMachineIDFake = 50;
	iMyMachineID = nMachineId;

	if (bMaxNameSize)
	{

		User = empty;
	}

	if (bJoinAsGhost)
	{
		User = empty;
		Name = empty;
	}

	pCAddPlayer = pThis;
	dN = User;
	dNN = Name;
	dT = unknown;
	dM = nMachineId;
	dP = a7;

	if (!lala) {
		fM = nMachineId;
		lala = true;
	}

	return CAddPlayerCommandTramp(pThis, User, Name, unknown, nMachineId, bHotjoin, a7);
}


void HookFunctions() {

	CSessionPostHook = CSessionPost(NULL /*address*/);

	MH_CreateHook(CSessionPostHook, &hkCSessionPost, (LPVOID*)&CSessionPostTramp);
	MH_EnableHook(CSessionPostHook);


	CAddPlayerCommandHook = GetCAddPlayerCommand(NULL /*address*/);

	MH_CreateHook(CAddPlayerCommandHook, &hkCAddPlayerCommand, (LPVOID*)&CAddPlayerCommandTramp);
	MH_EnableHook(CAddPlayerCommandHook);


	CStartGameCommandFunc = GetCStartGameCommand(NULL /*address*/);
	GetCCommandFunc = GetCCommand(NULL /*address*/);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	HookFunctions();
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}