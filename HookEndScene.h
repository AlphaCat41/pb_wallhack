#pragma once
#include "pch.h"

typedef HRESULT (APIENTRY* EndSceneFn)(LPDIRECT3DDEVICE9 pDevice);
EndSceneFn oEndScene = nullptr;

bool showMenu = true;
bool hpHackEnabled = false;
bool bulletHackEnabled = false;
bool fastHackEnabled = false;

HWND gameWindow = NULL;
WNDPROC oWndProc = NULL;
ID3DXFont* font = nullptr;

// 🟢 Hook EndScene
HRESULT APIENTRY hkEndScene (LPDIRECT3DDEVICE9 pDevice) {
	if (showMenu) {
		RECT rect = { 20, 20, 200, 100 };

		// 🔳 วาดพื้นหลังเมนู
		/*D3DRECT box = { rect.left, rect.top, rect.right, rect.bottom };
		pDevice->Clear (1, &box, D3DCLEAR_TARGET, D3DCOLOR_ARGB (180, 0, 0, 0), 0, 0);*/

		// 🔡 วาดข้อความสถานะ
		if (!font) {
			D3DXCreateFont (pDevice, 16, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
				TEXT ("Tahoma"), &font);
		}

		std::wstring lines[] = {
			L"[INSERT] Show Menu: " + std::wstring (showMenu ? L"ON" : L"OFF"),
			L"[F1] HP: " + std::wstring (hpHackEnabled ? L"ON" : L"OFF"),
			L"[F2] Bullet: " + std::wstring (bulletHackEnabled ? L"ON" : L"OFF"),
			L"[F3] Fast: " + std::wstring (fastHackEnabled ? L"ON" : L"OFF")
		};

		int startX = 30;
		int startY = 30;
		int lineHeight = 20; // ความสูงของแต่ละบรรทัด

		for (int i = 0; i < ARRAYSIZE (lines); ++i) {
			RECT textRect = {
				startX,
				startY + (i * lineHeight),
				startX + 200,
				startY + (i * lineHeight) + lineHeight
			};
			font->DrawTextW (NULL, lines[i].c_str (), -1, &textRect, DT_LEFT, D3DCOLOR_XRGB (255, 255, 255));
		}


	}

	return oEndScene (pDevice);
}

// 🧠 Hook WndProc สำหรับกดปุ่ม
LRESULT CALLBACK hkWndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_KEYDOWN) {
		if (wParam == VK_INSERT)
			showMenu = !showMenu;
		if (wParam == VK_F1)
			hpHackEnabled = !hpHackEnabled;
		if (wParam == VK_F2)
			bulletHackEnabled = !bulletHackEnabled;
		if (wParam == VK_F3)
			fastHackEnabled = !fastHackEnabled;
	}

	return CallWindowProc (oWndProc, hWnd, uMsg, wParam, lParam);
}