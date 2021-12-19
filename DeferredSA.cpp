
#include "CommonD.h"
#include "Lights.h"
#include "DeferredRenderer.h"
#include "EnvironmentMapping.h"
#include "VehicleMeshPipeline.h"
#include "SkinnedMeshPipeline.h"
#include "BuldingMeshPipeline.h"
#include "DefaultMeshPipeline.h"
#include "CGameIdle.h"
#include "Water.h"
#include "Quad.h"
#include "DefaultPipeline.h"
#include "Renderer.h"
#include "CascadedShadowRendering.h"
#include "ShadowCaster.h"
#include "CubemapReflection.h"
#include "DualParaboloidReflection.h"


#include "ShaderManager.h"
#include "SoftParticles.h"
#include "RenderableScene.h"

#include "ImguiManager.h"
#include "GameHooks.h"

void PipelinePlugins()
{
	SkinnedMeshPipe = new SkinnedMeshPipeline();
	DefaultMeshPipe = new DefaultMeshPipeline();
	BuldingMeshPipe = new BuldingMeshPipeline();
	VehicleMeshPipe = new VehicleMeshPipeline();
}
#include "PBSMaterial.h"

#include "SpotlightShadow.h"
void Initialize()
{
	CascadedShadowManagement = new CascadedShadowRendering();
	CascadedShadowManagement->Initialize();
	
	ShadowCasterEntity = new ShadowCaster();

	SpotShadow = new SpotlightShadow();
	SpotShadow->Initialize();

	CWaterLevel::InitShaders();
	EnvironmentMapping::InitializeGraphics();
	CubemapReflection::Initialize();
	DualParaboloidReflection::Initialize();

	Quad::Initialize();
	//CreateQuadRender();
	DeferredContext = new DeferredRendering();
	DeferredContext->Initialize();

	ShaderContext = new ShaderManager();

	SoftParticles::Initialize();
	PBSMaterialMgr::LoadMaterials();
	RenderableScene::InitGraphicsBuffer();

	ImguiManager::Initialize();
}

void SetSurfaceD(int id)
{
	auto rasterCube = RASTEREXTFROMCONSTRASTER(CubemapReflection::m_cubeRaster);
	auto rasterTex = RASTEREXTFROMCONSTRASTER(EnvironmentMapping::m_paraboloidRaster[1]);

	LPDIRECT3DCUBETEXTURE9 cube = (LPDIRECT3DCUBETEXTURE9)rasterCube->texture;

	IDirect3DSurface9* cubeSurface;
	cube->GetCubeMapSurface((D3DCUBEMAP_FACES)id, 0, &cubeSurface);

	LPSURFACE   surfaceLevelZ;
	IDirect3DTexture9_GetSurfaceLevel(rasterTex->texture, 0,&surfaceLevelZ);

	GetD3DDevice()->StretchRect(cubeSurface, 0, surfaceLevelZ, 0, D3DTEXF_NONE);
	cubeSurface->Release();
	surfaceLevelZ->Release();
}

bool g_bDrawGUI;
void Render()
{

	g_bDrawGUI = GetKeyState(VK_F12) & 1;

	ImguiManager::Bind();

	ImGui::Begin("Hello, world!");
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable;
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::BeginTabBar("MyTabBar", tab_bar_flags);
	//CWaterLevel::UpdateImgui();
	
	CascadedShadowManagement->UpdateImgui();
	DeferredContext->UpdateImgui();
	ImGui::EndTabBar();

	windowSize = ImVec2(100, 100);
	//SetSurfaceD(0);
	//ImGui::Image(EnvironmentMapping::m_paraboloidRaster[1], windowSize);
	//SetSurfaceD(1);   ImGui::NextColumn();
	//ImGui::Image(EnvironmentMapping::m_paraboloidRaster[1], windowSize);
	//SetSurfaceD(2);
	//ImGui::Image(EnvironmentMapping::m_paraboloidRaster[1], windowSize);
	ImGui::End();

	ImguiManager::Unbind();


	float weight = 512.0f* (float)RsGlobal.maximumHeight / (float)RsGlobal.maximumWidth;

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)SpotShadow->mColorRaster[0]);
	DrawScreenQuad(0, 0, weight, weight);

	/*MakeScreenQuad(0, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)DualParaboloidReflection::m_raster[0]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	MakeScreenQuad(weight, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)DualParaboloidReflection::m_raster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);*/
//	int weight = 512;
	
	/*SetSurfaceD(0);
	MakeScreenQuad(0, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	SetSurfaceD(1);
	MakeScreenQuad(weight, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	SetSurfaceD(2);
	MakeScreenQuad(weight *2, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	SetSurfaceD(3);
	MakeScreenQuad(0, weight, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	SetSurfaceD(4);
	MakeScreenQuad(weight, weight, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	SetSurfaceD(5);
	MakeScreenQuad(weight * 2, weight, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);*/

	//DrawDebug();
}



HHOOK hookHandle;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MessageProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(lParam & 0x80000000 || lParam & 0x40000000)
	{
		return CallNextHookEx(hookHandle, code, wParam, lParam);
	}

	if(code >= 0)
	{
		ImGui_ImplWin32_WndProcHandler(((LPMSG)lParam)->hwnd, ((LPMSG)lParam)->message, ((LPMSG)lParam)->wParam, ((LPMSG)lParam)->lParam);
	}
	return CallNextHookEx(hookHandle, code, wParam, lParam);
}

void GameProcess()
{
//	Lights::ClearLights();
	CGameIdle::UpdateShadowDNBalance();
}

void ShutdowRenderware()
{
	delete CascadedShadowManagement;
	//delete DeferredContext;
	delete ShaderContext;
	delete SkinnedMeshPipe;
	delete BuldingMeshPipe;
	delete DefaultMeshPipe;
	delete VehicleMeshPipe;

	
	Quad::Release();
	SoftParticles::Release();
}

#include "IndexBufferManager.h"
#include "RenderTargetManager.h"
void LostDevice()
{

	IndexBufferManager::Release();
}

void ResetDevice()
{
	//CascadedShadowManagement->UpdateTextures();
	CWaterLevel::UpdateTextures();
	IndexBufferManager::Restore();
	//DeferredContext->UpdateTextures();
	RenderTargetManager::Rebuild();
}

void Hook()
{
	SetWindowsHookEx(WH_GETMESSAGE, MessageProc, NULL, GetCurrentThreadId());

	plugin::Events::initRwEvent += Initialize;
	plugin::Events::attachRwPluginsEvent += PipelinePlugins;
	plugin::Events::gameProcessEvent += GameProcess;
	plugin::Events::shutdownRwEvent += ShutdowRenderware;
	plugin::Events::drawingEvent += Render;
	plugin::Events::d3dLostEvent += LostDevice;
	plugin::Events::d3dResetEvent += ResetDevice;
	GameHooks();
}