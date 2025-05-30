// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "HookDraw.h"
#include "HackMemory.h"
#include "HookEndScene.h"

DWORD WINAPI InitHook (LPVOID) {
	// รอให้เกมโหลด Direct3D9.dll
	while (!GetModuleHandleA ("d3d9.dll")) Sleep (100);
	Sleep (1000); // รอ Device สร้าง

	IDirect3D9* pD3D = Direct3DCreate9 (D3D_SDK_VERSION);
	if (!pD3D) return 0;

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetForegroundWindow ();

	IDirect3DDevice9* pDevice = nullptr;
	if (FAILED (pD3D->CreateDevice (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &pDevice))) {
		pD3D->Release ();
		return 0;
	}

	void** vTable = *reinterpret_cast<void***>(pDevice);

	MH_Initialize ();
	MH_CreateHook (vTable[82], &hkDrawIndexedPrimitive, reinterpret_cast<void**>(&oDrawIndexedPrimitive));
	MH_EnableHook (vTable[82]);
	MH_CreateHook (vTable[42], hkEndScene, (void**)&oEndScene);
	MH_EnableHook (vTable[42]);

	gameWindow = d3dpp.hDeviceWindow;
	oWndProc = (WNDPROC)SetWindowLongPtr (gameWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

	pDevice->Release ();
	pD3D->Release ();

	return 0;
}

BOOL APIENTRY DllMain (HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls (hModule);
		CreateThread (nullptr, 0, InitHook, nullptr, 0, nullptr);
		CreateThread (nullptr, 0, HackThread, nullptr, 0, nullptr);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

