#pragma once
#include "pch.h"
typedef HRESULT (APIENTRY* DrawIndexedPrimitive_t)(
	LPDIRECT3DDEVICE9 pDevice,
	D3DPRIMITIVETYPE Type,
	INT BaseVertexIndex,
	UINT MinVertexIndex,
	UINT NumVertices,
	UINT startIndex,
	UINT primCount);

DrawIndexedPrimitive_t oDrawIndexedPrimitive = nullptr;

// ������ҧ stride ����Ҩ��᷹�ѵ��
#define ENEMY_STRIDE 44

LPDIRECT3DTEXTURE9 yellowTexture = nullptr;

void CreateYellowTexture (LPDIRECT3DDEVICE9 pDevice) {
	if (yellowTexture) return; // ���ҧ�������Ǿ�

	if (SUCCEEDED (D3DXCreateTexture (pDevice, 1, 1, 1, 0, D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, &yellowTexture))) {
		D3DLOCKED_RECT lockedRect;
		if (SUCCEEDED (yellowTexture->LockRect (0, &lockedRect, nullptr, 0))) {
			// ������ͧ = 0xFFFFFF00 (AARRGGBB)
			DWORD* pColor = (DWORD*)lockedRect.pBits;
			*pColor = 0xFFFFFF00;
			yellowTexture->UnlockRect (0);
		}
	}
}

HRESULT APIENTRY hkDrawIndexedPrimitive (
	LPDIRECT3DDEVICE9 pDevice,
	D3DPRIMITIVETYPE Type,
	INT BaseVertexIndex,
	UINT MinVertexIndex,
	UINT NumVertices,
	UINT startIndex,
	UINT primCount) {
	// ��Ǩ�ͺ stride ���͵�Ǩ�Ѻ�����������
	LPDIRECT3DVERTEXBUFFER9 steam_data;
	UINT stride, offset;
	if (pDevice->GetStreamSource (0, &steam_data, &offset, &stride) == D3D_OK)
		steam_data->Release ();

	if (stride == ENEMY_STRIDE && stride != 0) {
		CreateYellowTexture (pDevice); // ���ҧ��������
		LPDIRECT3DTEXTURE9 originalTexture = nullptr;
		pDevice->GetTexture (0, (IDirect3DBaseTexture9**)&originalTexture);

		// �Դ Wallhack �¡�ûԴ Z-buffer
		pDevice->SetRenderState (D3DRS_ZENABLE, FALSE);
		pDevice->SetTexture (0, yellowTexture);         // �� Texture ������ͧ

		oDrawIndexedPrimitive (pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

		pDevice->SetRenderState (D3DRS_ZENABLE, TRUE);
		pDevice->SetTexture (0, originalTexture);

		if (originalTexture)
			originalTexture->Release ();
	}

	return oDrawIndexedPrimitive (pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}
