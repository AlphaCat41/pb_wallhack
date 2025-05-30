#pragma once
#include "pch.h"
#include "HookEndScene.h"

// -------- Configuration --------
struct PointerChain {
	int type; // 0 = hp, 1 = bullet
	uintptr_t baseAddress;
	std::vector<uintptr_t> offsets;
};

float hp = 200;
float bullet = 200;
float fast = 2; // 0 = normal, 2 = fast

// -------- Resolve Multi-level Pointer --------
uintptr_t ResolvePointer (uintptr_t base, const std::vector<uintptr_t>& offsets) {
	uintptr_t addr = *(uintptr_t*)base;
	for (size_t i = 0; i < offsets.size () - 1; i++) {
		if (!addr) return 0;
		addr = *(uintptr_t*)(addr + offsets[i]);
	}
	return addr + offsets.back (); // Final address to write
}

void PatchNOP (BYTE* address, size_t size) {
	DWORD oldProtect;
	VirtualProtect (address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset (address, 0x90, size); // Fill with NOPs (0x90)
	VirtualProtect (address, size, oldProtect, &oldProtect);
}

void WriteMemory () {
	uintptr_t baseAddress = (uintptr_t)GetModuleHandleA ("PointBlank.exe");
	if (!baseAddress) {
		MessageBoxA (NULL, "Failed to get module handle.", "Error", MB_ICONERROR | MB_OK);
		return; // Exit if module not found
	}
	std::vector<PointerChain> targets = {
		// hp
		{ 0, baseAddress + 0x010339A0, { 0x540, 0xC, 0x13C, 0x4 } },
		// bullet
		{ 1, baseAddress + 0x010339A0, { 0x2C, 0x200, 0x0 } },
		// fast
		{ 2, baseAddress + 0x010339A0, { 0x6A8, 0x30, 0x8B0 } },
	};

	while (true) {
		for (const auto& chain : targets) {
			uintptr_t targetAddress = ResolvePointer (chain.baseAddress, chain.offsets);
			if (targetAddress) {
				switch (chain.type) {
				case 0: // hp
					if (hpHackEnabled) *(float*)targetAddress = hp;
					break;
				case 1: // bullet
					if (bulletHackEnabled) *(float*)targetAddress = bullet;
					break;
				case 2: // fast
					if (fastHackEnabled) *(float*)targetAddress = fast;
				default:
					break;
				}
				//PatchNOP (reinterpret_cast<BYTE*>(targetAddress), 2);
			}
		}

		Sleep (100); // Don't hog CPU
	}
}
// -------- Thread Logic --------
DWORD WINAPI HackThread (LPVOID lpParam) {
	WriteMemory ();
	return 0;
}