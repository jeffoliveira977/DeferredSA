
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

//imgui
#include "imgui.h"
#include "imgui_impl_rw.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "ShaderManager.h"
#include "SoftParticles.h"
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
	CascadedShadowManagement->initGraphics();
	
	ShadowCasterEntity = new ShadowCaster();

	SpotShadow = new SpotlightShadow();
	SpotShadow->initGraphics();

	CWaterLevel::InitShaders();
	EnvironmentMapping::InitializeGraphics();

	//CreateQuadRender();
	DeferredContext = new DeferredRendering();
	DeferredContext->initGraphicsBuffer();

	ShaderContext = new ShaderManager();
	SoftParticlesContext = new SoftParticles();
	SoftParticlesContext->initGraphicsBuffer();
	PBSMaterialMgr::LoadMaterials();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::GetIO().MouseDrawCursor = true;
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(*(HWND*)0x00C8CF88);
	ImGui_ImplDX9_Init(GetD3DDevice());
}

void SetSurfaceD(int id)
{
	auto rasterCube = RASTEREXTFROMCONSTRASTER(EnvironmentMapping::m_cubeRaster);
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

static RwIm2DVertex screenQuad[4];
static RwImVertexIndex screenindices[6] = {0, 1, 2, 0, 2, 3};

static void MakeQuadTexCoords(bool textureSpace)
{
	float minU, minV, maxU, maxV;
	if(textureSpace)
	{
		minU = minV = 0.0f;
		maxU = maxV = 1.0f;
	}
	screenQuad[0].u = minU;
	screenQuad[0].v = minV;
	screenQuad[1].u = minU;
	screenQuad[1].v = maxV;
	screenQuad[2].u = maxU;
	screenQuad[2].v = maxV;
	screenQuad[3].u = maxU;
	screenQuad[3].v = minV;
}

static void
MakeScreenQuad(int x, int y, int width, int height)
{
	screenQuad[0].x = x;
	screenQuad[0].y = y;
	screenQuad[0].z = RWSRCGLOBAL(dOpenDevice).zBufferNear;
	screenQuad[0].rhw = 1.0f / 10;
	screenQuad[0].emissiveColor = 0xFFFFFFFF;
	screenQuad[1].x = x;
	screenQuad[1].y = y + height;
	screenQuad[1].z = screenQuad[0].z;
	screenQuad[1].rhw = screenQuad[0].rhw;
	screenQuad[1].emissiveColor = 0xFFFFFFFF;
	screenQuad[2].x = x + width;
	screenQuad[2].y = y + height;
	screenQuad[2].z = screenQuad[0].z;
	screenQuad[2].rhw = screenQuad[0].rhw;
	screenQuad[2].emissiveColor = 0xFFFFFFFF;
	screenQuad[3].x = x + width;
	screenQuad[3].y = y;
	screenQuad[3].z = screenQuad[0].z;
	screenQuad[3].rhw = screenQuad[0].rhw;
	screenQuad[3].emissiveColor = 0xFFFFFFFF;
	MakeQuadTexCoords(true);
}
#include "CRenderer.h"
#include "CScene.h"
#include "CFont.h"
#include "CWorld.h"
void DrawDebug()
{
	CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
	CRGBA color{255,255,255,255};//pi/360
	CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
	CFont::SetScale(0.5, 0.8);
	CFont::SetColor(color);
	float FontXPos = Scene.m_pRwCamera->frameBuffer->width - 20.0f;
	float FontYPos = Scene.m_pRwCamera->frameBuffer->height - 20.0f;
	CFont::PrintString(FontXPos, FontYPos - 600.0f, (char*)("Visible refl entities count: " + to_string(Renderer::ms_aVisibleReflectionObjects.size())).c_str());
	CFont::PrintString(FontXPos, FontYPos - 550.0f, (char*)("CameraHeading: " + to_string(CRenderer::ms_fCameraHeading)).c_str());
	CFont::PrintString(FontXPos, FontYPos - 500.0f, (char*)("Visible Entity count: " + to_string(CRenderer::ms_nNoOfVisibleEntities)).c_str());
	CFont::PrintString(FontXPos, FontYPos - 400.0f, (char*)("Visible Lod Entity count: " + to_string(CRenderer::ms_nNoOfVisibleLods)).c_str());
	CFont::PrintString(FontXPos, FontYPos - 300.0f, (char*)("Current scan code: " + to_string(CWorld::ms_nCurrentScanCode)).c_str());
	
}


bool g_bDrawGUI;
void Render()
{
	//RenderFrustum();
	g_bDrawGUI = GetKeyState(VK_F12) & 1;

	auto ped = FindPlayerPed(-1);
	auto pad = ped->GetPadFromPlayer();

	if(g_bDrawGUI)
		pad->DisablePlayerControls |= 0x20u;
	else
		pad->DisablePlayerControls &= 0xDFu;

	//ImGui_ImplDX9_NewFrame();
	ImGui_ImplRW_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hello, world!");
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable;
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::BeginTabBar("MyTabBar", tab_bar_flags);
	//CWaterLevel::imguiParameters();
	
	CascadedShadowManagement->imguiParameters();
	DeferredContext->imguiParameters();
	ImGui::EndTabBar();

	windowSize = ImVec2(100, 100);
	//SetSurfaceD(0);
	//ImGui::Image(EnvironmentMapping::m_paraboloidRaster[1], windowSize);
	//SetSurfaceD(1);   ImGui::NextColumn();
	//ImGui::Image(EnvironmentMapping::m_paraboloidRaster[1], windowSize);
	//SetSurfaceD(2);
	//ImGui::Image(EnvironmentMapping::m_paraboloidRaster[1], windowSize);
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();

	//ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	ImGui_ImplRW_RenderDrawData(ImGui::GetDrawData());
	
	int weight = 512;
	MakeScreenQuad(0, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_sphericalRaster);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);

	//MakeScreenQuad(weight, 0, weight, weight);
	//RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	//RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	/*int weight = 512;
	SetSurfaceD(0);
	MakeScreenQuad(0, 0, weight, weight);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);*/
	//SetSurfaceD(1);
	//MakeScreenQuad(weight, 0, weight, weight);
	//RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	//RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	//SetSurfaceD(2);
	//MakeScreenQuad(weight *2, 0, weight, weight);
	//RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	//RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	//SetSurfaceD(3);
	//MakeScreenQuad(0, weight, weight, weight);
	//RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	//RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	//SetSurfaceD(4);
	//MakeScreenQuad(weight, weight, weight, weight);
	//RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	//RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);
	//SetSurfaceD(5);
	//MakeScreenQuad(weight * 2, weight, weight, weight);
	//RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)EnvironmentMapping::m_paraboloidRaster[1]);
	//RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screenQuad, 4, screenindices, 6);

	//DrawDebug();
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
#include "CTimer.h"
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

void PrepareRealTimeShadows(const RwV3d& sundir)
{
	CascadedShadowManagement->CalculateShadowDistances(Scene.m_pRwCamera->nearPlane, Scene.m_pRwCamera->farPlane);

	// Render cascades
	for(int i = 0; i < CascadedShadowManagement->CascadeCount; i++)
		CascadedShadowManagement->DirectionalLightTransform(Scene.m_pRwCamera, sundir, i);
}

void RenderRealTimeShadows(const RwV3d& sundir)
{
	gRenderState = stageCascadeShadow;

	// Render cascades
	for(int i = 0; i < CascadedShadowManagement->CascadeCount; i++)
	{
		CascadedShadowManagement->RenderShadowToBuffer(i, [](int k)
		{
			ShadowCasterEntity->Render(k);
		});
	}
}

#include "CTimeCycle.h"

void CRealTimeShadowManager__Update()
{
	//ShadowCasterEntity->ClearCullList();
	ShadowCasterEntity->Update(GetSectorX(CRenderer::ms_vecCameraPosition.x), GetSectorY(CRenderer::ms_vecCameraPosition.y));
	//CVector g_vSunPosition;
	//GetSunPosn(&g_vSunPosition, CTimeCycle::m_CurrentColours.m_fFarClip);
	//const auto sunDirs = reinterpret_cast<RwV3d*>(0xB7CA50);
	//const auto curr_sun_dir = *reinterpret_cast<int*>(0xB79FD0);
	//const auto curr_sun_dirvec = &sunDirs[curr_sun_dir];
	////PrepareRealTimeShadows(sunDirs[curr_sun_dir]);
	//RenderRealTimeShadows(*(RwV3d*)&g_vSunPosition);
	// EnvironmentMapping::CubeMap();
	CascadedShadowManagement->Update();
	//SpotShadow->Update();
	EnvironmentMapping::SphericalMap();
}

#include "CMirrors.h"
void CMirrors__BeforeMainRender()
{
	
	//EnvironmentMapping::CubeMap();
	
	//SpotShadow->Update();
	//EnvironmentMapping::DualParaboloidMap();
	//CWaterLevel::RenderReflection(CRenderer_RenderRoadsAndBuildings);
	//CMirrors::BeforeMainRender();
}

void LostDevice()
{
	QuadDestroy();
}

void ResetDevice()
{
	CreateQuadRender();
}

void GameProcess()
{
	Lights::ClearLights();
	CGameIdle::UpdateShadowDNBalance();
}

void ShutdowRenderware()
{
	delete CascadedShadowManagement;
	delete DeferredContext;
	delete ShaderContext;
	delete SkinnedMeshPipe;
	delete BuldingMeshPipe;
	delete DefaultMeshPipe;
	delete VehicleMeshPipe;
}

#include "Immediate3D.h"

using namespace plugin;
HINSTANCE dllModule;

RwUInt32& SelectedMultisamplingLevels = *(RwUInt32*)0x008E2430;
RwUInt32& SelectedMultisamplingLevelsNonMask = *(RwUInt32*)0x008E2438;

RwBool RwD3D9ChangeMultiSamplingLevels2(RwUInt32)
{
	SelectedMultisamplingLevels = 0;
	SelectedMultisamplingLevelsNonMask = 0;
	return TRUE;
}

void RwD3D9EngineSetMultiSamplingLevels2(RwUInt32 numLevels)
{
	SelectedMultisamplingLevels = 0;
	SelectedMultisamplingLevelsNonMask = 0;
}
#include "VisibilityPlugins.h"

void Hook()
{
	//CImmediateRender__Patch();
	SetWindowsHookEx(WH_GETMESSAGE, MessageProc, NULL, GetCurrentThreadId());
	plugin::Events::initRwEvent += Initialize;
	plugin::Events::attachRwPluginsEvent += PipelinePlugins;
	plugin::Events::gameProcessEvent += GameProcess;
	plugin::Events::shutdownRwEvent += ShutdowRenderware;

	//plugin::Events::processScriptsEvent += GameProcess;
	//plugin::Events::drawingEvent += Render;

	plugin::Events::d3dLostEvent += LostDevice;
	plugin::Events::d3dResetEvent += ResetDevice;

	// Deferred shading don't work with multi sampling, so disable it.
	patch::RedirectJump(0x007F8A90, RwD3D9ChangeMultiSamplingLevels2);
	patch::RedirectJump(0x007F84F0, RwD3D9EngineSetMultiSamplingLevels2);

	// Remove stencil shadows and sets new shadow mapping
	plugin::patch::Nop(0x0053C1AB, 5); // CStencilShadows::Process

	//plugin::patch::RedirectJump(0x00706AB0, CRealTimeShadowManager__Update);
	//plugin::patch::RedirectCall(0x0053EA12, CMirrors__BeforeMainRender);
	VisibilityPlugins::Patch();
	Immediate3D__Hook();
	 SoftParticlesContext->hook();

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
	CWaterLevel::Hook();
    CGameIdle::Patch();
    VehicleMeshPipeline::hook();
    BuldingMeshPipeline::hook();
    DefaultMeshPipeline::hook();
	SkinnedMeshPipeline::hook();
}