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

void GameHooks()
{	
	// Deferred shading don't work with multi sampling, so disable it.
	patch::RedirectJump(0x007F8A90, rwD3D9ChangeMultiSamplingLevels);
	patch::RedirectJump(0x007F84F0, rwD3D9EngineSetMultiSamplingLevels);

	// Remove stencil shadows
	plugin::patch::Nop(0x0053C1AB, 5); // CStencilShadows::Process

	plugin::patch::RedirectJump(0x00706AB0, CRealTimeShadowManager__Update);
	plugin::patch::RedirectCall(0x0053EA12, CMirrors__BeforeMainRender);

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

	Immediate3D::Hook();
	SoftParticles::Hook();
    CWaterLevel::Hook();
    CGameIdle::Patch();
    VehicleMeshPipeline::Hook();
    BuldingMeshPipeline::Hook();
    DefaultMeshPipeline::Hook();
    SkinnedMeshPipeline::Hook();
}