#include "plugin.h"
#include "VehicleMeshPipeline.h"
#include "SkinnedMeshPipeline.h"
#include "BuldingMeshPipeline.h"
#include "DefaultMeshPipeline.h"
#include "CGameIdle.h"
#include "Immediate3D.h"
#include "Water.h"
#include "CTimer.h"
#include "ShadowCaster.h"
#include "RenderableScene.h"
#include "CascadedShadowRendering.h"
#include "CubemapReflection.h"
#include "DualParaboloidReflection.h"
#include "SoftParticles.h"
#include "Renderer.h"
#include "DefaultPipeline.h"
#include "Lights.h"

using namespace plugin;

void RsMouseSetPos_hook(RwV2d* screen)
{
	if(1)
	{
		POINT pos;
		GetCursorPos(&pos);
		screen->x = pos.x;
		screen->y = pos.y;
	}

	RsMouseSetPos(screen);
	CTimer::m_UserPause = g_bDrawGUI;

}
#include "CScene.h"
#include "CCamera.h"




void CRealTimeShadowManager__Update()
{
	if (CGame::currArea == 0 && CGameIdle::m_fShadowDNBalance <= 1.0)
	{
		CascadedShadowManagement->CalculateShadowDistances(Scene.m_pRwCamera->nearPlane, Scene.m_pRwCamera->farPlane);
		const auto sunDirs = reinterpret_cast<RwV3d*>(0xB7CA50);
		const auto curr_sun_dir = *reinterpret_cast<int*>(0xB79FD0);
		const auto curr_sun_dirvec = &sunDirs[curr_sun_dir];
		CascadedShadowManagement->DirectionalLightTransform(Scene.m_pRwCamera, sunDirs[curr_sun_dir], 0);
	}

	ShadowCasterEntity->Update(1, 1);

	CascadedShadowManagement->Update();

	CubemapReflection::Update();
	CubemapReflection::RenderScene();

	//DualParaboloidReflection::Update();
	//DualParaboloidReflection::RenderScene();

	//RenderableReflectionObjects::Update();

	RenderableScene::m_frustumRenderable->SetWorldMatrix(XMMatrixInverse(0, CascadedShadowManagement->Desc[0].mLightViewMatrix));
	RenderableScene::m_frustumRenderable->SetProjectionMatrix(CascadedShadowManagement->Desc[0].mLightOrthoMatrix, true);

	//RenderableScene::m_frustumRenderable->SetWorldMatrix(XMMatrixInverse(0, CubemapReflection::m_viewMatrix[2]));
	//RenderableScene::m_frustumRenderable->SetProjectionMatrix(CubemapReflection::m_projectionMatrix, false);

	RenderableScene::Render();
}

void CMirrors__BeforeMainRender()
{
	CWaterLevel::RenderReflection(CRenderer_RenderRoadsAndBuildings);
}

RwUInt32& SelectedMultisamplingLevels = *(RwUInt32*)0x008E2430;
RwUInt32& SelectedMultisamplingLevelsNonMask = *(RwUInt32*)0x008E2438;

RwBool rwD3D9ChangeMultiSamplingLevels(RwUInt32)
{
	SelectedMultisamplingLevels = 0;
	SelectedMultisamplingLevelsNonMask = 0;
	return TRUE;
}

void rwD3D9EngineSetMultiSamplingLevels(RwUInt32)
{
	SelectedMultisamplingLevels = 0;
	SelectedMultisamplingLevelsNonMask = 0;
}

unsigned int gCurrentLevel;
RwRaster* gCurrentRaster;

RwUInt8* LockLevel(RwRaster* raster, unsigned int level, unsigned int mode)
{
	gCurrentLevel = level;
	gCurrentRaster = raster;
	return RwRasterLock(raster, level, mode);
}

RwUInt32 ReadLevel(RwStream* stream, void* pbuf, unsigned int size)
{
	// if we have a wrong level (and this texture is not corrupted)
	if(size == 0 && gCurrentLevel != 0)
	{
		auto rasterExt = RASTEREXTFROMRASTER(gCurrentRaster);

		// simply copy the data from previous level
		if(gCurrentRaster->cType == rwRASTERTYPETEXTURE && rasterExt->compressed && rasterExt->texture)
		{
			D3DLOCKED_RECT rect;
			rasterExt->texture->LockRect(gCurrentLevel - 1, &rect, NULL, D3DLOCK_READONLY);

			RwUInt32 sz = 16;
			if(rasterExt->d3dFormat == D3DFMT_DXT1)
				sz = 8;

			memcpy(pbuf, rect.pBits, sz);

			rasterExt->texture->UnlockRect(gCurrentLevel - 1);
			return size;
		}
	}
	return RwStreamRead(stream, pbuf, size);
}

void SetGeometryUsageFlag(RpGeometry* geometry, unsigned int flags)
{
	unsigned int usageFlags = RpD3D9GeometryGetUsageFlags(geometry);

	usageFlags |= flags;
	RpD3D9GeometrySetUsageFlags(geometry, (RpD3D9GeometryUsageFlag)usageFlags);
}
RpGeometry* CreateGeometryCheckNormals(int numVerts, int numTriangles, unsigned int format)
{
	RpGeometry* geometry;
	format |= rpGEOMETRYNORMALS;

	geometry = RpGeometryCreate(numVerts, numTriangles, format);
	SetGeometryUsageFlag(geometry, rpD3D9GEOMETRYUSAGE_CREATETANGENTS);

	return geometry;
}

float fNewFarClip = 500.0f;

void CLODLightManager__DrawDistanceChanger()
{

	// float fNewFarClip = 500.0f;
	static DWORD* pPlayerPed = (DWORD*)0xB6F5F0;
	float& CTimeCycle__m_fCurrentFarClip = *(float*)(0xB7C4F0);

	float fMinDrawDistanceOnTheGround = 1000.0;
	float fMaxPossibleDrawDistance = 5000.0;
	float fFactor1 = 8.0;
	float fFactor2 = 3.0;
	if (*pPlayerPed)
	{
		if (*(BYTE*)((*pPlayerPed) + 0x2F) != 8) //[byte] Location status
		{
			//if (*ActiveInterior == 0)
			//{

			/*if (CWeather__UnderWaterness <= 0.339731634f)
				fNewFarClip = (fFactor1 / fFactor2) * (TheCamera.GetPosition().z) + fMinDrawDistanceOnTheGround;
			else
				fNewFarClip = CTimeCycle__m_fCurrentFarClip;*/
			//}
		}
	}
}
#include "CTimeCycle.h"
void __cdecl HookedCameraUpdateZShiftScale(RwCamera* camera) {
	CLODLightManager__DrawDistanceChanger();
	//if (camera->farPlane == CTimeCycle::m_CurrentColours.m_fFarClip) {
		//float newFarClip = *p2dfxFarClip;

		//if (newFarClip < 200.0f) newFarClip = 200.0f;
		//else if (newFarClip > 2000.0f) newFarClip = 2000.0f;

		camera->farPlane = fNewFarClip;
		//*p2dfxFarClip = newFarClip;
		CTimeCycle::m_CurrentColours.m_fFarClip = fNewFarClip;
	//}
	Call<0x7EE200, RwCamera*>(camera);
}

void StoreRealTimeShadow(CPhysical* physical, float displacementX, float displacementY,
	float frontX, float frontY, float sideX, float sideY)
{

}
void StoreShadowForPole(CEntity* entity, float offsetX, float offsetY, float offsetZ, 
	float poleHeight, float poleWidth, uint32_t localId)
{

}

void StoreShadowForPedObject(CEntity* ped, float displacementX, float displacementY, 
	float frontX, float frontY, float sideX, float sideY)
{

}

#include "CRenderer.h"
void GameHooks()
{
	//injector::MakeCALL(0x53EBE4, CLODLightManager__DrawDistanceChanger, true);

	//patch::RedirectCall(0x7EE2B0, HookedCameraUpdateZShiftScale);
	//injector::WriteMemory(0x40C524, &fNewFarClip, true);
	//injector::WriteMemory(0x553F79, &fNewFarClip, true);
	//injector::WriteMemory(0x5556A7, &fNewFarClip, true);
	//injector::WriteMemory(0x732515, &fNewFarClip, true);

	//injector::WriteMemory(0x53D532, &fNewFarClip, true);
	//injector::WriteMemory(0x53DC7B, &fNewFarClip, true);
	//injector::WriteMemory(0x53DCB8, &fNewFarClip, true);
	//injector::WriteMemory(0x53EA95, &fNewFarClip, true);

	plugin::patch::RedirectJump(0x00707CA0, StoreRealTimeShadow);
	plugin::patch::RedirectJump(0x0070C750, StoreShadowForPole);
	plugin::patch::RedirectJump(0x00707B40, StoreShadowForPedObject);

	patch::RedirectCall(0x4CDCA4, LockLevel);
	patch::RedirectCall(0x4CDCD9, ReadLevel);

	// Deferred shading don't work with multi sampling, so disable it.
	patch::RedirectJump(0x007F8A90, rwD3D9ChangeMultiSamplingLevels);
	patch::RedirectJump(0x007F84F0, rwD3D9EngineSetMultiSamplingLevels);

	// Remove stencil shadows
	plugin::patch::Nop(0x0053C1AB, 5); // CStencilShadows::Process

	plugin::patch::Nop(0x006E15E2, 5); // car headlight shadow
	plugin::patch::Nop(0x006E170F, 5);// car headlight shadow

	plugin::patch::Nop(0x0070C3F8, 5); // car light shadow
	plugin::patch::Nop(0x0070C33F, 5); // car light shadow

	plugin::patch::Nop(0x006FD42C, 5); // corona shadow
	plugin::patch::Nop(0x006FD47C, 5); // corona shadow

	plugin::patch::Nop(0x00553A04, 5); // CShadows::RenderExtraPlayerShadows

	//plugin::patch::RedirectJump(0x00706AB0, CRealTimeShadowManager__Update);
	//plugin::patch::RedirectCall(0x0053EA12, CMirrors__BeforeMainRender);
	//plugin::patch::RedirectJump(0x00734570, Renderer::InsertEntityIntoSortedList);
	//plugin::patch::RedirectJump(0x005534B0, Renderer::AddEntityToRenderList);
	//plugin::patch::RedirectJump(0x00553710, Renderer::AddToLodRenderList);
	//plugin::patch::RedirectJump(0x00553260, Renderer::RenderOneNonRoad);
	//plugin::patch::RedirectJump(0x00553AA0, Renderer::RenderEverythingBarRoads);
	//plugin::patch::RedirectJump(0x553910, Renderer::PreRender);
	//plugin::patch::RedirectJump(0x00553A10, Renderer::RenderRoads);
	plugin::patch::RedirectJump(0x553E40, Renderer::SetupLightingForEntity);
	//plugin::patch::RedirectJump(0x005556E0, Renderer::ConstructRenderList);
	//plugin::patch::RedirectJump(0x00553910, Renderer::PreRender);

//	plugin::patch::Nop(0x535FCD, 5);

	plugin::patch::RedirectCall(0x0053E9F1, RsMouseSetPos_hook);

	Lights::Patch();
	plugin::patch::RedirectJump(0x7578C0, D3D9AtomicDefaultInstanceCallback);

	plugin::patch::RedirectCall(0x74D234, CreateGeometryCheckNormals);
	plugin::patch::RedirectCall(0x5D71D9, SetGeometryUsageFlag);

	Immediate3D::Hook();
	SoftParticles::Hook();
    CWaterLevel::Hook();
    CGameIdle::Patch();
    VehicleMeshPipeline::Hook();
    BuldingMeshPipeline::Hook();
    DefaultMeshPipeline::Hook();
    SkinnedMeshPipeline::Hook();
}