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

void CRealTimeShadowManager__Update()
{
	ShadowCasterEntity->Update(1, 1);

	CascadedShadowManagement->Update();

	CubemapReflection::Update();
	CubemapReflection::RenderScene();

	//DualParaboloidReflection::Update();
	//DualParaboloidReflection::RenderScene();

	//RenderableReflectionObjects::Update();

	RenderableScene::m_frustumRenderable->SetWorldMatrix(XMMatrixInverse(0, CascadedShadowManagement->Desc[0].lightViewMatrix));
	RenderableScene::m_frustumRenderable->SetProjectionMatrix(CascadedShadowManagement->Desc[0].lightOrthoMatrix, true);

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


void GameHooks()
{
	patch::RedirectCall(0x4CDCA4, LockLevel);
	patch::RedirectCall(0x4CDCD9, ReadLevel);

	// Deferred shading don't work with multi sampling, so disable it.
	patch::RedirectJump(0x007F8A90, rwD3D9ChangeMultiSamplingLevels);
	patch::RedirectJump(0x007F84F0, rwD3D9EngineSetMultiSamplingLevels);

	// Remove stencil shadows
	plugin::patch::Nop(0x0053C1AB, 5); // CStencilShadows::Process

	//plugin::patch::RedirectJump(0x00706AB0, CRealTimeShadowManager__Update);
	//plugin::patch::RedirectCall(0x0053EA12, CMirrors__BeforeMainRender);
	//plugin::patch::RedirectJump(0x00734570, Renderer::InsertEntityIntoSortedList);
	//plugin::patch::RedirectJump(0x005534B0, Renderer::AddEntityToRenderList);
	//plugin::patch::RedirectJump(0x00553710, Renderer::AddToLodRenderList);
	//plugin::patch::RedirectJump(0x00553260, Renderer::RenderOneNonRoad);
	//plugin::patch::RedirectJump(0x00553AA0, Renderer::RenderEverythingBarRoads);
	//plugin::patch::RedirectJump(0x00553A10, Renderer::RenderRoads);
	plugin::patch::RedirectJump(0x005556E0, Renderer::ConstructRenderList);
	plugin::patch::RedirectJump(0x00553910, Renderer::PreRender);

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