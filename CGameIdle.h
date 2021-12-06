#pragma once
#include "CGame.h"
void CRenderer_RenderRoadsAndBuildings(void);
class CGameIdle
{
public:
	static void UpdateShadowDNBalance();
	static void Patch();
	static void Initialize();
	static void RenderReflections(CVector vPositon, int id);
	static void RenderScene();
	static void Idle(void *Data);
	static void RenderCubeMapReflections(void (*renderCB)());
	static void Release();
	static void Post();
	static RwRaster* CubeRaster;
	static RwTexture* ReflectionTex;
	static IDirect3DCubeTexture9* m_cubemap;
	static RwCamera* m_pReflCam;
	static RwFrame* m_pReflFrame;
	static RwRaster *m_pReflectionRT[2];
	static bool m_bUseAlphaTestForTexAlpha;
	static float m_fShadowDNBalance;
};
extern bool g_bDrawGUI;
