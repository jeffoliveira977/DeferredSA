#include "CGameIdle.h"
#include "DeferredRenderer.h"
#include "Lights.h"
#include "Water.h"
#include "EnvironmentMapping.h"
#include "CCamera.h"
#include "Renderer.h"

void nullsub(void*){

}
RpWorld* InitWorld(RwBBox*bbox){
	bbox->inf.x *= 2;
	bbox->inf.y *= 2;
	bbox->inf.z *= 2;
	bbox->sup.x *= 2;
	bbox->sup.y *= 2;
	bbox->sup.z *= 2;
	return RpWorldCreate(bbox);
}

void Idle_hook();


void _declspec(naked) HOOK_Render3DStuff()
{
	_asm
	{
		pushad
		call Idle_hook
		popad
		push 0x53EAD8
		ret
	}
}

void CRenderer_RenderRoadsAndBuildings(void);
void RenderReflection(void)
{
	ObjectType.buldings = true;
	ObjectType.vehicles = false;
	ObjectType.objects = true;
	ObjectType.peds = false;

	Renderer::ConstructRenderList();

	EnvironmentMapping::SetRenderCallback(CRenderer_RenderRoadsAndBuildings);
	 EnvironmentMapping::CubeMap();
	//EnvironmentMapping::DualParaboloidMap();
	//EnvironmentMapping::SphericalMap();

	ObjectType.peds = true;
	ObjectType.buldings = true;
	ObjectType.vehicles = true;
	ObjectType.objects = true;
	CWaterLevel::RenderReflection(CRenderer_RenderRoadsAndBuildings);
}

#define AttachInputDevices() ((void(__cdecl*)())0x00744A20)()
#define AttachPlugins() ((bool(__cdecl*)())0x0053D870)()
#define Terminate3D() ((void(__cdecl*)())0x0053D910)()
#define Initialize3D() ((bool(__cdecl*)())0x005BF390)()
#define RWInitialize() ((bool(__cdecl*)())0x00619600)()

#define FrontendIdle() ((void(__cdecl*)())0x0053E770)()
//#define Idle() ((void(__cdecl*)())0x0053E920)() 

#include "CScene.h"
#include "CDraw.h"

 void RenderEffects();
 void CGameIdle::Patch()
 {
	 plugin::patch::RedirectCall(0x53EABF, RenderScene);
	 plugin::patch::RedirectJump(0x53E170, RenderEffects);
	// plugin::patch::Nop(0x53EAC4, 0x5);
	// plugin::patch::Nop(0x53EAC9, 0x5);
	// plugin::patch::RedirectCall(0x53E9F9, RenderReflection);
	 //plugin::patch::RedirectJump(0x53EABF, HOOK_Render3DStuff);
	 // plugin::patch::RedirectCall(0x53ECBD, Idle);
	// plugin::patch::RedirectCall(0x5BD71C, InitWorld);
	 //plugin::patch::SetInt(0x619626, 60);
	 //plugin::patch::RedirectCall(0x53C1AB, nullsub);
 }


#include "CVisibilityPlugins.h"
#include "CObjectInfo.h"

#include <game_sa\CVehicleModelInfo.h>
#include <game_sa\CModelInfo.h>


void RenderForward()
{
	CCoronas__RenderReflections();
	CCoronas__Render();
	CWeather__RenderRainStreaks();
	CCoronas__RenderSunReflection();	
}
#include "Fx_c.h"
void RenderEffects()
{
	DefinedState();
	CBirds__Render();
	CSkidmarks__Render();
	CRopes__Render();
	CGlass__Render();
	CVisibilityPlugins__RenderReallyDrawLastObjects();
	CCoronas__Render();
	g_fx.Render(TheCamera.m_pRwCamera, 0);
	CWaterCannons__Render();
	CWaterLevel__RenderWaterFog();
	//CClouds__MovingFogRender();
	//CClouds__VolumetricCloudsRender();
	//if(CHeli__NumberOfSearchLights || CTheScripts__NumberOfScriptSearchLights)
	//{
	//	CHeli__Pre_SearchLightCone();
	//	CHeli__RenderAllHeliSearchLights();
	//	CTheScripts__RenderAllSearchLights();
	//	CHeli__Post_SearchLightCone();
	//}
	CWeaponEffects__Render();
	if(CReplay__Mode != 1 && !CPad__GetPad(0)->DisablePlayerControls)
	{
		CPlayerPed__DrawTriangleForMouseRecruitPed(FindPlayerPed(-1));
	}
	CSpecialFX__Render();
	//CPointLights__RenderFogEffect();
	CRenderer__RenderFirstPersonVehicle();
	//CPostEffects__Render();
	//DrawWaterWave();
}
#include "CScene.h"
#include "CSprite2d.h"
#include "CMenuManager.h"
#include "CDraw.h"
#include "CWeather.h"
#include "CRenderer.h"
#include "CWorld.h"
#include "CTimeCycle.h"
#include "CFont.h"
#include "CMessages.h"
#include "CCredits.h"
#include "CTimer.h"
#include "CPointLights.h"
#include "CHud.h"
#include "CascadedShadowRendering.h"
#include "CClouds.h"
#include "VisibilityPlugins.h"

void RenderDeferred()
{
	Renderer::RenderRoads();
	Renderer::RenderEverythingBarRoads();
	//Renderer::RenderTOBJs();
	BreakManager_c__Render(g_breakMan, 0);

	CRenderer::RenderFadingInUnderwaterEntities();
	 CRenderer::RenderFadingInEntities();

	BreakManager_c__Render(g_breakMan, 1);

	//CVisibilityPlugins::RenderWeaponPedsForPC();
	VisibilityPlugins::RenderWeaponPedsNoMuzzleFlash();
}

float CGameIdle::m_fShadowDNBalance=1.0;
#include "CClock.h"

void CGameIdle::UpdateShadowDNBalance()
{
	// TODO: make times adjustable perhaps?
	float currentMinutes = (CClock::ms_nGameClockMinutes + 60.0f * CClock::ms_nGameClockHours) + CClock::ms_nGameClockSeconds / 60.0f;
	// if time is less than 7 am then it's night
	if(currentMinutes < 360.0)
	{
		m_fShadowDNBalance = 1.0;
		return;
	}
	if(currentMinutes < 420.0)
	{
		m_fShadowDNBalance = (420.0f - currentMinutes) / 60.0f;
		return;
	}
	// if time is between 7 am and 7 pm than it's day
	if(currentMinutes < 1080.0)
	{
		m_fShadowDNBalance = 0.0;
		return;
	}
	// else it's night
	if(currentMinutes >= 1140.0)
		m_fShadowDNBalance = 1.0;
	else
		m_fShadowDNBalance = 1.0f - (1140.0f - currentMinutes) / 60.0f;

}

#include "ShaderManager.h"
#include "SoftParticles.h"

#define RenderStencil() ((unsigned char (__cdecl *)())0x7113B0)()
#define RenderGrass() ((void (__cdecl *)())0x5DBAE0)()
#define DoRWRenderHorizon() ((void (__cdecl *)())0x7178F0)()
#define RenderStoredShadows() ((void (__cdecl *)())0x70A960)()
#define sub_707F40() ((void (__cdecl *)())0x707F40)()
#define RenderStaticShadows() ((void (__cdecl *)())0x708300)()
#include "LightManager.h"
#include "RenderableSphere.h"
RenderableSphere* mRenderableSphere=0;
void CGameIdle::RenderScene(){

	ShaderContext->Update();

	RwD3D9RenderStateReset();
	DefinedState();
	//RenderForward();

	{
		DeferredContext->Start();
		RenderDeferred();
		DeferredContext->Stop();
	}

	gRenderState = stageForward;
	//DefinedState();
	//RwD3D9RenderStateReset();
	RenderDeferred();

	//ShaderContext->SetViewProjectionMatrix(4, true);

	//if (mRenderableSphere == 0)
	//{
	//	mRenderableSphere = new RenderableSphere();
	//	mRenderableSphere->Initialize(10, 10);
	//}
	//
	//auto coors = FindPlayerCoors(-1);

	//CEntity* entity = FindPlayerEntity(-1);
	//if (entity)
	//{
	//	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)1);
	//	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)2);
	//	CColModel* col = entity->GetColModel();
	//	if (col != nullptr)
	//	{
	//		XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));
	//		XMMATRIX translation = XMMatrixTranslation(coors.x, coors.y, coors.z);
	//		uint32_t maxLights = min((size_t)29, gLightManager.GetPointLightCount());
	//		XMINT4 SolidWhite = { 0, 255, 255, 255 };
	//		for (int i = 0; i < maxLights; i++)
	//		{
	//			auto light = gLightManager.GetPointLightAt(i);
	//			auto radius = light->GetRadius();
	//			auto intensity = light->GetIntensity();
	//			auto color = light->GetColor();

	//			XMINT4 color2 = { (int)(color.x * 255), (int)(color.y * 255), (int)(color.z * 255), 255 };
	//			mRenderableSphere->SetColor(color2);

	//			Math::BoundingSphere sphere;
	//			sphere.Center = light->GetPosition();
	//			sphere.Radius = radius;
	//			//sphere.Center = *reinterpret_cast<XMFLOAT3*>(&col->m_boundSphere.m_vecCenter);
	//			//	sphere.Radius = col->m_boundSphere.m_fRadius;
	//			mRenderableSphere->SetSphere(sphere);
	//			mRenderableSphere->SetWorldMatrix(light->GetWorld());
	//			RwD3D9SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//			mRenderableSphere->Render();
	//			RwD3D9SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//			/*PrintMessage("%f %f %f", col->m_boundSphere.m_fRadius, sphere.Radius, sphere.Center.z);*/
	//		}
	//	}
	//}

	sub_707F40();
	RenderStaticShadows();
	RenderStoredShadows();	

	DefinedState();
	CWaterLevel::SetupWaterShader(); 
	CPlantMgr__Render();
	RenderForward();
	DeferredContext->RenderPostProcessing();

}

void Idle_hook()
{
	CGameIdle::RenderScene();

	/*RwD3D9RenderStateReset();

	DefinedState();

	RenderForward();
	gRenderState = RenderingStage::stageDeferred;
	DeferredRendering::RenderToGeometryBuffer(CGameIdle::RenderScene);
	DefinedState();
	DeferredRendering::RenderToScreenOutput();

	gRenderState = stageForward;
	RwD3D9RenderStateReset();
	DefinedState();
	CGameIdle::RenderScene();*/

	/*RwD3D9RenderStateReset();
	DefinedState();*/
	//CWaterLevel::SetupWaterShader();
	/*RwD3D9RenderStateReset();
	DefinedState();
	RenderEffects();*/
}

//
//void PrepareRealTimeShadows(const RwV3d& sundir)
//{
//	CascadedShadowManagement->CalculateShadowDistances(Scene.m_pRwCamera->nearPlane, Scene.m_pRwCamera->farPlane);
//	
//	// Render cascades
//	for(int i = 0; i < CascadedShadowManagement->CascadeCount; i++)
//		CascadedShadowManagement->DirectionalLightTransform(Scene.m_pRwCamera, sundir, i);
//}
//
//void RenderRealTimeShadows(const RwV3d& sundir)
//{
//	gRenderState = stageCascadeShadow;
//
//	// Render cascades
//	for(int i = 0; i < CascadedShadowManagement->CascadeCount; i++)
//	{
//		CascadedShadowManagement->RenderShadowToBuffer(i, [](int k)
//		{
//			Renderer::RenderShadowCascade(k);
//		});
//	}
//}

void CGameIdle::Idle(void* param)
{
	CGame__TimeMillisecondsFromStart = CTimer__GetTimeMillisecondsFromStart();
	CTimer::Update();
	CSprite2d::InitPerFrame();
	CFont::InitPerFrame();
	CPointLights::NumLights = 0;
	Lights::ClearLights();
	CGame::Process();
	CAudioEngine__Service(AudioEngine);

	SetLightsWithTimeOfDayColour(Scene.m_pRpWorld);

	if(!param)
		return;

	CDraw::CalculateAspectRatio();
	CameraSize(Scene.m_pRwCamera, 0, tanf(CDraw::ms_fFOV * (3.1415927f / 360.0f)), CDraw::ms_fAspectRatio);
	CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);
	RwCameraClear(Scene.m_pRwCamera, gColourTop, rwCAMERACLEARZ);
	if(!RsCameraBeginUpdate(Scene.m_pRwCamera))
		return;

	if(!FrontEndMenuManager.m_bMenuActive && TheCamera.GetScreenFadeStatus() != 2)
	{
		
		const auto sunDirs = reinterpret_cast<RwV3d*>(0xB7CA50);		// Sun direction table pointer(sun directions is always the same)
		const auto curr_sun_dir = *reinterpret_cast<int*>(0xB79FD0);	// Current sun direction id
		const auto curr_sun_dirvec = &sunDirs[curr_sun_dir];			// Current sun direction vector
	
		//
		RwCameraEndUpdate(Scene.m_pRwCamera);
		TheCamera.m_mViewMatrix.Update();
		RwRenderStateSet(rwRENDERSTATESTENCILENABLE, FALSE);
	
		//PrepareRealTimeShadows(sunDirs[curr_sun_dir]);
		Renderer::ConstructRenderList();
		Renderer::PreRender();
		CWorld::ProcessPedsAfterPreRender();
		
		EnvironmentMapping::SetRenderCallback(Renderer::RenderCubemapEntities);
		EnvironmentMapping::CubeMap();

		//RenderRealTimeShadows(sunDirs[curr_sun_dir]);
		CascadedShadowManagement->Update();
		//RwCameraBeginUpdate(Scene.m_pRwCamera);
		
		/*RwV2d mousePos;
		mousePos.x = RsGlobal.maximumWidth * 0.5f;
		mousePos.y = RsGlobal.maximumHeight * 0.5f;

		RsMouseSetPos(&mousePos);*/

		if(CWeather::LightningFlash)
		{
			CTimeCycle::m_CurrentColours.m_nSkyBottomRed = 255;
			CTimeCycle::m_CurrentColours.m_nSkyBottomGreen = 255;
			CTimeCycle::m_CurrentColours.m_nSkyBottomBlue = 255;
			if(!DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255))
				return;
		}
		else if(!DoRWStuffStartOfFrame_Horizon(CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue,
											   CTimeCycle::m_CurrentColours.m_nSkyBottomRed, CTimeCycle::m_CurrentColours.m_nSkyBottomGreen, CTimeCycle::m_CurrentColours.m_nSkyBottomBlue, 255))
			return;


		RwCameraSetFarClipPlane(Scene.m_pRwCamera, CTimeCycle::m_CurrentColours.m_fFarClip);
		Scene.m_pRwCamera->fogPlane = CTimeCycle::m_CurrentColours.m_fFogStart;
		
		DefinedState();
		//TheCamera.m_mViewMatrix.Update();
		//CClouds::RenderSkyPolys();
	
		

		RwD3D9RenderStateReset();
		DefinedState();
		RenderForward();

	/*	gRenderState = stageDeferred;
		DeferredRendering::RenderToGeometryBuffer(RenderDeferred);
		DefinedState();
		DeferredRendering::RenderToScreenOutput();*/

		gRenderState = stageForward;
		RwD3D9RenderStateReset();
		DefinedState();
		RenderDeferred();

		DefinedState();
		CWaterLevel::SetupWaterShader();
		DefinedState();
		RenderEffects();
		Render2dStuff();
	}

	if(FrontEndMenuManager.m_bMenuActive)
		CMenuManager__DrawFrontEnd(FrontEndMenuManager);

	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);

	DoFade();
	CHud::DrawAfterFade();
	CMessages::Display(0);
	CFont::DrawFonts();

	if(CCredits::bCreditsGoing)
	{
		if(!FrontEndMenuManager.m_bMenuActive)
			CCredits::Render();
	}

	FlushObrsPrintfs();
	RwCameraEndUpdate(Scene.m_pRwCamera);
	RsCameraShowRaster(Scene.m_pRwCamera);
}