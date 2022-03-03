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
#include "SpotlightShadow.h"
#include "PointLightShadow.h"

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


#include "DeferredRenderer.h"

void CRealTimeShadowManager__Update()
{
	//if (CGameIdle::m_fShadowDNBalance)
	//{
	
	PointShadow->Update();
	SpotShadow->Update();

	//}

	/*if (CGame::currArea == 0 && CGameIdle::m_fShadowDNBalance <= 1.0)
	{
		CascadedShadowManagement->CalculateShadowDistances(Scene.m_pRwCamera->nearPlane, Scene.m_pRwCamera->farPlane);
		const auto sunDirs = reinterpret_cast<RwV3d*>(0xB7CA50);
		const auto curr_sun_dir = *reinterpret_cast<int*>(0xB79FD0);
		const auto curr_sun_dirvec = &sunDirs[curr_sun_dir];
		CascadedShadowManagement->DirectionalLightTransform(Scene.m_pRwCamera, sunDirs[curr_sun_dir], 0);
	}*/

	//ShadowCasterEntity->Update(1, 1);
	static CPerfTimer shadowTimer("Shadow Time");
	shadowTimer.Start();
	 CascadedShadowManagement->Update();
	 shadowTimer.Stop();

	 //PrintMessage("%s", shadowTimer.GetTimerResult().c_str());

	CubemapReflection::Update();
	 CubemapReflection::RenderScene();

	//DualParaboloidReflection::Update();
	//DualParaboloidReflection::RenderScene();

	//RenderableReflectionObjects::Update();
	 gRenderState = stageReflectionCubemap;
	RenderableScene::m_frustumRenderable->SetWorldMatrix(XMMatrixInverse(0, CascadedShadowManagement->Desc[0].mLightViewMatrix));
	RenderableScene::m_frustumRenderable->SetProjectionMatrix(CascadedShadowManagement->Desc[0].mLightOrthoMatrix, true);
	RenderableScene::Render();
	/*RenderableScene::m_frustumRenderable->SetWorldMatrix(XMMatrixInverse(0, CubemapReflection::m_viewMatrix[0]));
	RenderableScene::m_frustumRenderable->SetProjectionMatrix(CubemapReflection::m_projectionMatrix, false);

	RenderableScene::Render();*/
}

void CMirrors__BeforeMainRender()
{
	CWaterLevel::RenderReflection(CRenderer_RenderRoadsAndBuildings);
}

RwUInt32& SelectedMultisamplingLevels2 = *(RwUInt32*)0x008E2430;
RwUInt32& SelectedMultisamplingLevelsNonMask2 = *(RwUInt32*)0x008E2438;

RwBool rwD3D9ChangeMultiSamplingLevels(RwUInt32)
{
	SelectedMultisamplingLevels2 = 0;
	SelectedMultisamplingLevelsNonMask2 = 0;
	return TRUE;
}

void rwD3D9EngineSetMultiSamplingLevels(RwUInt32)
{
	SelectedMultisamplingLevels2 = 0;
	SelectedMultisamplingLevelsNonMask2 = 0;
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


#include "CTimeCycle.h"


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

void __cdecl HookedCameraUpdateZShiftScale(RwCamera* camera) {
	//if (camera->farPlane == CTimeCycle::m_CurrentColours.m_fFarClip) {
		float newFarClip = camera->farPlane;

		if (newFarClip < 200.0f) newFarClip = 200.0f;
		//else if (newFarClip > 2000.0f) newFarClip = 2000.0f;

		camera->farPlane = newFarClip;
		CTimeCycle::m_CurrentColours.m_fFarClip = newFarClip;
	//}
	Call<0x7EE200, RwCamera*>(camera);
}

#include "CRenderer.h"
#include "LightManager.h"
#include "RenderingEngine.h"
#include "RenderStates.h"
#include "Raster.h"


void GameHooks()
{
	//patch::RedirectCall(0x7EE2B0, HookedCameraUpdateZShiftScale);
	
	// redirect hook, because RenderHook redirects Idle
	//uintptr_t p = injector::ReadMemory<uintptr_t>(0x53EBE4 + 1) + 0x53EBE4;
	//injector::WriteMemory<uintptr_t>(0x53C215 + 1, p - 0x53C215);

	plugin::patch::RedirectJump(0x00707CA0, StoreRealTimeShadow);
	plugin::patch::RedirectJump(0x0070C750, StoreShadowForPole);
	plugin::patch::RedirectJump(0x00707B40, StoreShadowForPedObject);

	//patch::RedirectCall(0x4CDCA4, LockLevel);
	//patch::RedirectCall(0x4CDCD9, ReadLevel);

	// Deferred shading don't work with multi sampling, so disable it.
	patch::RedirectJump(0x007F8A90, rwD3D9ChangeMultiSamplingLevels);
	patch::RedirectJump(0x007F84F0, rwD3D9EngineSetMultiSamplingLevels);

	// Remove stencil shadows
	plugin::patch::Nop(0x0053C1AB, 5); // CStencilShadows::Process

	//plugin::patch::Nop(0x006E15E2, 5); // car headlight shadow
	//plugin::patch::Nop(0x006E170F, 5);// car headlight shadow

	//plugin::patch::Nop(0x0070C3F8, 5); // car light shadow
	//plugin::patch::Nop(0x0070C33F, 5); // car light shadow

	//plugin::patch::Nop(0x006FD42C, 5); // corona shadow
	//plugin::patch::Nop(0x006FD47C, 5); // corona shadow
	
	gLightManager.Hook();
	plugin::patch::Nop(0x00553A04, 5); // CShadows::RenderExtraPlayerShadows

	plugin::patch::RedirectJump(0x00706AB0, CRealTimeShadowManager__Update);
	//plugin::patch::RedirectCall(0x0053EA12, CMirrors__BeforeMainRender);
	
	 Renderer::Hook();
	plugin::patch::Nop(0x006FD42C, 5);
	plugin::patch::Nop(0x006FD47C, 5);

	plugin::patch::RedirectCall(0x0053E9F1, RsMouseSetPos_hook);

	plugin::patch::RedirectJump(0x7578C0, D3D9AtomicDefaultInstanceCallback);

	plugin::patch::RedirectCall(0x74D234, CreateGeometryCheckNormals);
	plugin::patch::RedirectCall(0x5D71D9, SetGeometryUsageFlag);

	auto registerDevice = []()
	{
		plugin::patch::RedirectJump(0x7F5500, DeferredRenderingEngine::RenderingEngine::CreateVertexBuffer);
		plugin::patch::RedirectJump(0x7F56A0, DeferredRenderingEngine::RenderingEngine::DestroyVertexBuffer);
		plugin::patch::RedirectJump(0x7F5CB0, DeferredRenderingEngine::RenderingEngine::VertexBufferManagerOpen);
		plugin::patch::RedirectJump(0x7F5D20, DeferredRenderingEngine::RenderingEngine::VertexBufferManagerClose);
		plugin::patch::RedirectJump(0x7F5940, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferManagerCreate);
		plugin::patch::RedirectJump(0x7F5A00, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferCreate);
		plugin::patch::RedirectJump(0x7F5AE0, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferDestroy);
		plugin::patch::RedirectJump(0x7F5B10, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferLock);
		plugin::patch::RedirectJump(0x7F5C90, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferUnlock);
		plugin::patch::RedirectJump(0x7F5840, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferRelease);
		plugin::patch::RedirectJump(0x7F58D0, DeferredRenderingEngine::RenderingEngine::DynamicVertexBufferRestore);
		plugin::patch::RedirectJump(0x7FAA30, DeferredRenderingEngine::RenderingEngine::CreateVertexDeclaration);
		plugin::patch::RedirectJump(0x7FAC10, DeferredRenderingEngine::RenderingEngine::DeleteVertexDeclaration);
		plugin::patch::RedirectJump(0x7F7730, DeferredRenderingEngine::RenderingEngine::CameraClear);
		plugin::patch::RedirectJump(0x7F8F20, DeferredRenderingEngine::RenderingEngine::CameraBeginUpdate);
		plugin::patch::RedirectJump(0x7F98D0, DeferredRenderingEngine::RenderingEngine::CameraEndUpdate);
		plugin::patch::RedirectJump(0x7F99B0, DeferredRenderingEngine::RenderingEngine::RasterShowRaster);
		plugin::patch::RedirectJump(0x7F6CB0, DeferredRenderingEngine::RenderingEngine::D3D9SetPresentParameters);
		plugin::patch::RedirectJump(0x7F8640, DeferredRenderingEngine::RenderingEngine::RwD3D9ChangeVideoMode);
		plugin::patch::RedirectJump(0x7F6B00, DeferredRenderingEngine::RenderingEngine::D3D9ClearCacheShaders);
		plugin::patch::RedirectJump(0x7F6B40, DeferredRenderingEngine::RenderingEngine::D3D9ClearCacheMatrix);
		plugin::patch::RedirectJump(0x7F5F70, DeferredRenderingEngine::RenderingEngine::D3D9System);
		plugin::patch::RedirectJump(0x7F9F30, DeferredRenderingEngine::RenderingEngine::SetFVF);
		plugin::patch::RedirectJump(0x7FA1C0, DeferredRenderingEngine::RenderingEngine::SetIndices);
		plugin::patch::RedirectJump(0x7FA030, DeferredRenderingEngine::RenderingEngine::SetStreamSource);
		plugin::patch::RedirectJump(0x7FA090, DeferredRenderingEngine::RenderingEngine::SetStreams);
		plugin::patch::RedirectJump(0x7F9FF0, DeferredRenderingEngine::RenderingEngine::SetPixelShader);
		plugin::patch::RedirectJump(0x7F9FB0, DeferredRenderingEngine::RenderingEngine::SetVertexShader);
		plugin::patch::RedirectJump(0x7F9F70, DeferredRenderingEngine::RenderingEngine::SetVertexDeclaration);
		plugin::patch::RedirectJump(0x7FA390, DeferredRenderingEngine::RenderingEngine::SetTransform);
		plugin::patch::RedirectJump(0x7FA520, DeferredRenderingEngine::RenderingEngine::SetTransformWorld);
		plugin::patch::RedirectJump(0x7FA4F0, DeferredRenderingEngine::RenderingEngine::GetTransform);
		plugin::patch::RedirectJump(0x7F9E90, DeferredRenderingEngine::RenderingEngine::SetRenderTarget);
		plugin::patch::RedirectJump(0x7F5F20, DeferredRenderingEngine::RenderingEngine::_SetRenderTarget);
		plugin::patch::RedirectJump(0x7F9E80, DeferredRenderingEngine::RenderingEngine::GetCurrentD3DRenderTarget);
		plugin::patch::RedirectJump(0x7F5EF0, DeferredRenderingEngine::RenderingEngine::SetDepthStencilSurface);
		plugin::patch::RedirectJump(0x7FA320, DeferredRenderingEngine::RenderingEngine::DrawIndexedPrimitive);
		plugin::patch::RedirectJump(0x7FA1F0, DeferredRenderingEngine::RenderingEngine::DrawIndexedPrimitiveUP);
		plugin::patch::RedirectJump(0x7FA360, DeferredRenderingEngine::RenderingEngine::DrawPrimitive);
		plugin::patch::RedirectJump(0x7FA290, DeferredRenderingEngine::RenderingEngine::DrawPrimitiveUP);
		plugin::patch::RedirectJump(0x7FAA00, DeferredRenderingEngine::RenderingEngine::IndexBuffer32bitsCreate);
	};

	auto registerRenderStates = []()
	{
		plugin::patch::RedirectJump(0x7FC2D0, RenderStates::SetRenderState);
		plugin::patch::RedirectJump(0x7FC320, RenderStates::GetRenderState);
		plugin::patch::RedirectJump(0x7FC340, RenderStates::SetTextureStageState);
		plugin::patch::RedirectJump(0x7FC3A0, RenderStates::GetTextureStageState);
		plugin::patch::RedirectJump(0x7FC3C0, RenderStates::SetSamplerState);
		plugin::patch::RedirectJump(0x7FC400, RenderStates::GetSamplerState);
		plugin::patch::RedirectJump(0x7FC430, RenderStates::RwD3D9SetMaterial);
		plugin::patch::RedirectJump(0x7FC4D0, RenderStates::RwD3D9SetSurfaceProperties);
		plugin::patch::RedirectJump(0x7FCAC0, RenderStates::_rwD3D9RenderStateOpen);
		plugin::patch::RedirectJump(0x7FD100, RenderStates::_rwD3D9RenderStateReset);
		plugin::patch::RedirectJump(0x7FD810, RenderStates::_rwD3D9RWGetRenderState);
		plugin::patch::RedirectJump(0x7FDA90, RenderStates::_rwD3D9RenderStateTextureAddress);
		plugin::patch::RedirectJump(0x7FDB00, RenderStates::_rwD3D9RenderStateTextureAddressU);
		plugin::patch::RedirectJump(0x7FDB40, RenderStates::_rwD3D9RenderStateTextureAddressV);
		plugin::patch::RedirectJump(0x7FDB80, RenderStates::_rwD3D9RenderStateTextureFilter);
		plugin::patch::RedirectJump(0x7FDC30, RenderStates::_rwD3D9RenderStateSrcBlend);
		plugin::patch::RedirectJump(0x7FDC80, RenderStates::_rwD3D9RenderStateDestBlend);
		plugin::patch::RedirectJump(0x7FDCD0, RenderStates::_rwD3D9RWSetRasterStage);
		plugin::patch::RedirectJump(0x7FDE50, RenderStates::_rwD3D9RWGetRasterStage);
		plugin::patch::RedirectJump(0x7FC200, RenderStates::RenderStateFlushCache);
		plugin::patch::RedirectJump(0x7FDE60, RenderStates::_rwD3D9SetTextureAnisotropyOffset);
		plugin::patch::RedirectJump(0x7FDE70, RenderStates::RwD3D9SetTexture);
		plugin::patch::RedirectJump(0x7FE0A0, RenderStates::_rwD3D9RenderStateVertexAlphaEnable);
		plugin::patch::RedirectJump(0x7FE190, RenderStates::_rwD3D9RenderStateIsVertexAlphaEnable);
		plugin::patch::RedirectJump(0x7FECC0, RenderStates::rwD3D9RenderStateBorderColor);
		plugin::patch::RedirectJump(0x7FEC70, RenderStates::rwD3D9RenderStateShadeMode);
		plugin::patch::RedirectJump(0x7FED00, RenderStates::rwD3D9RenderStateCullMode);
		plugin::patch::RedirectJump(0x7FEA70, RenderStates::rwD3D9RenderStateZWriteEnable);
		plugin::patch::RedirectJump(0x7FEB70, RenderStates::rwD3D9RenderStateZTestEnable);
		plugin::patch::RedirectJump(0x7FE1A0, RenderStates::_rwD3D9RenderStateStencilEnable);
		plugin::patch::RedirectJump(0x7FE1F0, RenderStates::_rwD3D9RenderStateStencilFail);
		plugin::patch::RedirectJump(0x7FE240, RenderStates::_rwD3D9RenderStateStencilZFail);
		plugin::patch::RedirectJump(0x7FE290, RenderStates::_rwD3D9RenderStateStencilPass);
		plugin::patch::RedirectJump(0x7FE2E0, RenderStates::_rwD3D9RenderStateStencilFunction);
		plugin::patch::RedirectJump(0x7FE330, RenderStates::_rwD3D9RenderStateStencilFunctionRef);
		plugin::patch::RedirectJump(0x7FE380, RenderStates::_rwD3D9RenderStateStencilFunctionMask);
		plugin::patch::RedirectJump(0x7FE3D0, RenderStates::_rwD3D9RenderStateStencilFunctionWriteMask);
		plugin::patch::RedirectJump(0x7FE420, RenderStates::_rwD3D9RWSetRenderState);
	};

	auto registerRaster = []()
	{
		plugin::patch::RedirectJump(0x4C9F90, Raster::D3D9RasterLock);
		plugin::patch::RedirectJump(0x4CA290, Raster::D3D9RasterUnlock);
		plugin::patch::RedirectJump(0x4CC580, Raster::D3D9CheckRasterFormat);
		plugin::patch::RedirectJump(0x4CC1D0, Raster::D3D9FindCorrectRasterFormat);
		plugin::patch::RedirectJump(0x4CD820, Raster::_rwD3D9NativeTextureRead);
		plugin::patch::RedirectJump(0x4C9ED0, Raster::_rwD3D9CubeRasterCreate);
		plugin::patch::RedirectJump(0x4CB7C0, Raster::rwD3D9CreateTextureRaster);
		plugin::patch::RedirectJump(0x4CCE60, Raster::_rwD3D9RasterCreate);
		plugin::patch::RedirectJump(0x4CBB00, Raster::_rwD3D9RasterDestroy);
		plugin::patch::RedirectJump(0x4CCD70, Raster::rwD3D9CreateZBufferRaster);
		plugin::patch::RedirectJump(0x4CB9C0, Raster::rwD3D9CreateCameraTextureRaster);
		plugin::patch::RedirectJump(0x4CB640, Raster::_rxD3D9VideoMemoryRasterListRelease);
		plugin::patch::RedirectJump(0x4CC970, Raster::_rxD3D9VideoMemoryRasterListRestore);
		plugin::patch::RedirectJump(0x4CB5E0, Raster::rxD3D9VideoMemoryRasterListDestroy);
		plugin::patch::RedirectJump(0x4CB560, Raster::rxD3D9VideoMemoryRasterListRemove);
		plugin::patch::RedirectJump(0x4CB530, Raster::rxD3D9VideoMemoryRasterListAdd);
		plugin::patch::RedirectJump(0x4C9AB0, Raster::_rwD3D9RasterPluginAttach);
		plugin::patch::RedirectJump(0x4CC150, Raster::_rwD3D9RasterOpen);
		plugin::patch::RedirectJump(0x4CC170, Raster::_rwD3D9RasterClose);
		plugin::patch::RedirectJump(0x81F360, Raster::RwD3D9RasterStreamReadDDS);
		plugin::patch::RedirectJump(0x820A10, Raster::RwD3D9DDSTextureRead);
	};

	registerDevice();
	registerRenderStates();
	registerRaster();

	Immediate3D::Hook();
	SoftParticles::Hook();
    CWaterLevel::Hook();
    CGameIdle::Patch();
    VehicleMeshPipeline::Hook();
    BuldingMeshPipeline::Hook();
    DefaultMeshPipeline::Hook();
    SkinnedMeshPipeline::Hook();
}