#include "DeferredRenderer.h"
#include "EnvironmentMapping.h"
#include "CubemapReflection.h"
#include "DualParaboloidReflection.h"
#include "Lights.h"
#include "Quad.h"
#include "CTimeCycle.h"
#include "CascadedShadowRendering.h"
#include "CScene.h"
#include "ShaderManager.h"
#include "CGame.h"
#include "CGameIdle.h"
#include <DirectXMath.h>
#include "ShaderConstant.h"
#include "VolumetricClouds.h"
#include "PixelShader.h"

RenderingStage gRenderState;
DeferredRendering *DeferredContext;


void DeferredRendering::Initialize()
{
	mPointSpotLightPS = unique_ptr<PixelShader>(new PixelShader());
	mPointSpotLightPS->CreateFromBinary("DeferredPointAndSpotLightPS");

	mDirectLightPS = unique_ptr<PixelShader>(new PixelShader());
	mDirectLightPS->CreateFromBinary("DeferredDirectLightPS");

	mCombineLightPS = unique_ptr<PixelShader>(new PixelShader());
	mCombineLightPS->CreateFromBinary("DeferredFinalPassPS");

	mAtmosphereScaterringPS = unique_ptr<PixelShader>(new PixelShader());
	mAtmosphereScaterringPS->CreateFromBinary("AtmosphericScattering");

	mScreenRT = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mScreenRT->Initialize();

	mGraphicsLight = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGraphicsLight->Initialize();

	for(size_t i = 0; i < 4; i++)
	{
		mGraphicsBuffer[i] = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
		mGraphicsBuffer[i]->Initialize();
	}

	mVolumetricClouds = unique_ptr<VolumetricClouds>(new VolumetricClouds());
	mVolumetricClouds->Initialize();

	mPostProcessing = unique_ptr<PostProcessing>(new PostProcessing());
	mPostProcessing->Initialize();

	mVolumetricLight = unique_ptr<VolumetricLight>(new VolumetricLight());
	mVolumetricLight->Initialize();

	mAmbientOcclusion = unique_ptr<AmbientOcclusion>(new AmbientOcclusion());
	mAmbientOcclusion->Initialize();
}

void DeferredRendering::Start()
{
	gRenderState = stageDeferred;

	RwRaster* rasters[] = {
		mGraphicsBuffer[0]->GetRaster(),
		mGraphicsBuffer[1]->GetRaster(),
		mGraphicsBuffer[2]->GetRaster(),
		mGraphicsBuffer[3]->GetRaster()};

	rwD3D9SetRenderTargets(rasters, 4, 0);
	ShaderContext->SetViewProjectionMatrix(4, true);
	ShaderContext->SetViewMatrix(4);
}

void DeferredRendering::Stop()
{
	DefinedState();

	ShaderContext->SetInverseViewMatrix(0);
	ShaderContext->SetProjectionMatrix(4);

	for(size_t i = 0; i < 4; i++)
	{
		rwD3D9SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		rwD3D9SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		rwD3D9SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	}

	_rwD3D9RWSetRasterStage(mGraphicsLight->GetRaster(), 0);
	_rwD3D9RWSetRasterStage(mGraphicsBuffer[0]->GetRaster(), 1);
	_rwD3D9RWSetRasterStage(mGraphicsBuffer[1]->GetRaster(), 2);
	_rwD3D9RWSetRasterStage(mGraphicsBuffer[2]->GetRaster(), 3);
	_rwD3D9RWSetRasterStage(mGraphicsBuffer[3]->GetRaster(), 4);

	// We need to disable Z buffer
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	mAmbientOcclusion->Render();

	RwD3D9SetRenderTarget(0, mGraphicsLight->GetRaster());

	RenderDirectLight();
	RenderPointAndSpotLight();

	// Restore render target and draw light to final raster
	RwD3D9RestoreRenderTargets(4);
	RenderFinalPass();

	AtmosphericScattering();


	mVolumetricClouds->Render(mScreenRT.get());
	mVolumetricLight->Render(mScreenRT.get());

	_rwD3D9SetPixelShader(NULL);
	_rwD3D9SetVertexShader(NULL);
}

void DeferredRendering::RenderPostProcessing()
{
	//DefinedState();
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	mPostProcessing->RenderFXAA(mScreenRT.get());
	//mPostProcessing->RenderBloom(mScreenRT);
}

void DeferredRendering::RenderDirectLight()
{
	ShaderContext->SetTimecyProps(8);

	CascadedShadowManagement->SetParamsBuffer();

	ShaderContext->SetSunDirection(11);
	ShaderContext->SetFogParams(12);

	if(!CGame::currArea && CGameIdle::m_fShadowDNBalance < 1.0)
	{
		for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
			rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 6);

		_rwD3D9SetPixelShaderConstant(13, &CascadedShadowManagement->mConstantBuffer,
									  sizeof(CascadedShadowManagement->mConstantBuffer) / sizeof(XMVECTOR));

	}

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	mDirectLightPS->Apply();
	Quad::Render();
}

#include "CCamera.h"

void DeferredRendering::RenderPointAndSpotLight()
{
	_rwD3D9SetPixelShaderConstant(8, &Scene.m_pRwCamera->farPlane, 1);
	
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	mPointSpotLightPS->Apply();
	Lights::SortByDistance(TheCamera.GetPosition().ToRwV3d());

	XMVECTOR value[4];
	
	// Spot and point light 
	for(int i = 0; i < Lights::m_nLightCount; i++)
	{
		memcpy(value, &Lights::Buffer()[i], sizeof(LightData));
		_rwD3D9SetPixelShaderConstant(9, value, 
									  sizeof(LightData)); 

		Quad::Render();
	}
}

void DeferredRendering::RenderFinalPass()
{
	_rwD3D9SetPixelShaderConstant(8, EnvironmentMapping::m_paraboloidBasis, 4);

	ShaderContext->SetTimecyProps(12);

	RwRaster* rasters[] = {
		CubemapReflection::m_cubeRaster,
		EnvironmentMapping::m_sphericalRaster,
		DualParaboloidReflection::m_raster[0],
		DualParaboloidReflection::m_raster[1],
		mAmbientOcclusion->mSSAORaster};

	for(size_t i = 0; i < 5; i++)
	{
		rwD3D9SetSamplerState(i + 4, D3DSAMP_BORDERCOLOR, 0x0);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
		rwD3D9RWSetRasterStage(rasters[i], i + 4);
	}

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	mCombineLightPS->Apply();
	Quad::Render();
}

void DeferredRendering::AtmosphericScattering()
{
	ShaderContext->SetTimecyProps(8);

	mScreenRT->CopyFromSurface(nullptr);
	
	_rwD3D9RWSetRasterStage(mScreenRT->GetRaster(), 4);

	mAtmosphereScaterringPS->Apply();
	Quad::Render();;
}

#include "imgui.h"
void DeferredRendering::UpdateImgui()
{
	mVolumetricLight->UpdateImgui();
	mAmbientOcclusion->UpdateImgui();

	/*if(ImGui::BeginTabItem("Cloud"))
	{
		ImGui::EndTabItem();
		ImGui::InputFloat("VolumeBox_top", &VolumeBox_top, 2.0, 100.0);
		ImGui::InputFloat("VolumeBox_bottom", &VolumeBox_bottom, 2, 100.0);

		ImGui::InputFloat("Atomesphere_Distance", &Atomesphere_Distance, 100.0, 200.0);
		ImGui::InputFloat("Atomesphere_Smoothness", &Atomesphere_Smoothness, 100, 200.0);
		ImGui::InputFloat3("Atomesphere_Smoothness", Atomesphere, "%.1f");

		ImGui::InputFloat3("cloud_shift", cloud_shift, "%.1f");

		ImGui::InputFloat("BodyTop", &BodyTop,2.0, 100.0);
		ImGui::InputFloat("BodyMiddle", &BodyMiddle, 2, 100.0);
		ImGui::InputFloat("BodyBottom", &BodyBottom, 2.0, 100.0);
		ImGui::InputFloat("BodyThickness", &BodyThickness, 0.01, 0.1, "%.1f");
	}*/
}
D3DPRESENT_PARAMETERS& Present = *(D3DPRESENT_PARAMETERS*)0xC9C040;


void DeferredRendering::UpdateTextures()
{
}