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
#include "SpotlightShadow.h"
#include "PointLightShadow.h"
RenderingStage gRenderState;
DeferredRendering *DeferredContext;


void DeferredRendering::Initialize()
{
	mPointLightPS = unique_ptr<PixelShader>(new PixelShader());
	mPointLightPS->CreateFromBinary("DeferredPointLightPS");

	mSpotLightPS = unique_ptr<PixelShader>(new PixelShader());
	mSpotLightPS->CreateFromBinary("DeferredPointAndSpotLightPS");

	mDirectLightPS = unique_ptr<PixelShader>(new PixelShader());
	mDirectLightPS->CreateFromBinary("DeferredDirectLightPS");

	mCombineLightPS = unique_ptr<PixelShader>(new PixelShader());
	mCombineLightPS->CreateFromBinary("DeferredFinalPassPS");

	mAtmosphereScaterringPS = unique_ptr<PixelShader>(new PixelShader());
	mAtmosphereScaterringPS->CreateFromBinary("AtmosphericScattering");

	mScreenRT = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A8R8G8B8));
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

	RenderLights();
	AtmosphericScattering();


	//mVolumetricClouds->Render(mScreenRT.get());
	//mVolumetricLight->Render(mScreenRT.get());

	_rwD3D9SetPixelShader(NULL);
	_rwD3D9SetVertexShader(NULL);
}

void DeferredRendering::RenderPostProcessing()
{
	DefinedState();
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//mPostProcessing->RenderFXAA(mScreenRT.get());
	//mPostProcessing->RenderBloom(mScreenRT);
}

#include "CCamera.h"
#include "LightManager.h"
#include "BoundingSphere.h"
void DeferredRendering::RenderLights()
{
	ShaderContext->SetTimecyProps(8);
	ShaderContext->SetSunDirection(11);
	ShaderContext->SetFogParams(12);

	if(!CGame::currArea && CGameIdle::m_fShadowDNBalance < 1.0)
	{
		CascadedShadowManagement->UpdateBuffer();

		for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
			rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 6);

		_rwD3D9SetPixelShaderConstant(13, &CascadedShadowManagement->mConstantBuffer,
									  sizeof(CascadedShadowManagement->mConstantBuffer) / sizeof(float[4]));

	}

	RwD3D9SetRenderTarget(0, mGraphicsLight->GetRaster());

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	mDirectLightPS->Apply();
	Quad::Render();
	//return;

	_rwD3D9SetPixelShaderConstant(8, &CTimeCycle::m_CurrentColours.m_fFarClip, 1);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	XMMATRIX view, projection;
	RwD3D9GetTransform(D3DTS_VIEW, &view);
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);
	//BoundingFrustum frustum;
	//frustum.CreateFromMatrix(frustum, projection);
	//frustum.Transform(frustum, XMMatrixInverse(0, view));

	Math::Frustum frustum;
	frustum.SetMatrix(view * projection);

	mPointLightPS->Apply();
	gLightManager.SortByDistance({ TheCamera.GetPosition().x,TheCamera.GetPosition().y ,TheCamera.GetPosition().z });
	int m = 0, n = 0;
	for (int i = 0; i < gLightManager.GetPointLightCount(); i++)
	{
		auto light = gLightManager.GetPointLightAt(i);

		auto radius = light.GetRadius();
		auto intensity = light.GetIntensity();

		//RwSphere sphere;
		Math::BoundingSphere sphere;
		sphere.Center.x = light.GetPosition().x ;
		sphere.Center.y = light.GetPosition().y ;
		sphere.Center.z = light.GetPosition().z;
		sphere.Radius = light.GetRadius();

		//if (RwCameraFrustumTestSphere(Scene.m_pRwCamera, &sphere) !=RwFrustumTestResult::rwSPHEREOUTSIDE)
		if(frustum.Intersects(sphere))
		{
			rwD3D9SetSamplerState(5, D3DSAMP_BORDERCOLOR, 0x0);
			rwD3D9SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			rwD3D9SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			rwD3D9SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			rwD3D9SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			rwD3D9SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			rwD3D9SetSamplerState(5, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			//_rwD3D9RWSetRasterStage(PointShadow->mColorRaster[i], 5);

			_rwD3D9SetPixelShaderConstant(9, &light.GetPosition(), 1);
			_rwD3D9SetPixelShaderConstant(10, &light.GetDirection(), 1);
			_rwD3D9SetPixelShaderConstant(11, &light.GetColor(), 1);
			_rwD3D9SetPixelShaderConstant(12, &radius, 1);
			_rwD3D9SetPixelShaderConstant(13, &intensity, 1);
			//_rwD3D9SetPixelShaderConstant(14, &(light.GetViewMatrix(0) * light.GetProjection()), 4);
			Quad::Render();
			m++;
		}
		else
			n++;
	}

	/*mSpotLightPS->Apply();
	for (size_t i = 0; i < gLightManager.GetSpotLightCount(); i++)
	{
		auto light = gLightManager.GetSpotLightAt(i);

		auto radius = light.GetRadius();
		auto intensity = light.GetIntensity();
		auto coneAngle = light.GetAngle();

		rwD3D9SetSamplerState(5, D3DSAMP_BORDERCOLOR, 0x0);
		rwD3D9SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		rwD3D9SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		rwD3D9SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		rwD3D9SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(5, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
		 _rwD3D9RWSetRasterStage(SpotShadow->mColorRaster[i], 5);

		_rwD3D9SetPixelShaderConstant(9, &light.GetPosition(), 1);
		_rwD3D9SetPixelShaderConstant(10, &light.GetDirection(), 1);
		_rwD3D9SetPixelShaderConstant(11, &light.GetColor(), 1);
		_rwD3D9SetPixelShaderConstant(12, &radius, 1);
		_rwD3D9SetPixelShaderConstant(13, &intensity, 1);
		_rwD3D9SetPixelShaderConstant(14, &coneAngle, 1);
		_rwD3D9SetPixelShaderConstant(15, &(light.GetViewMatrix() * light.GetProjection()) , 4);

		Quad::Render();
	}*/


	 uint maxlight = 0;
	maxlight = max(maxlight, gLightManager.GetPointLightCount());
	PrintMessage("total: %d culled: %d not culled: %d", maxlight, n, m);


	// Render to default surface
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);

	for (size_t i = 1; i < 4; i++)
		__rwD3D9SetRenderTarget(i, NULL);

	_rwD3D9SetPixelShaderConstant(8, EnvironmentMapping::m_paraboloidBasis, 4);

	ShaderContext->SetTimecyProps(12);

	RwRaster* rasters[] = {
		CubemapReflection::m_cubeRaster,
		EnvironmentMapping::m_sphericalRaster,
		DualParaboloidReflection::m_raster[0],
		DualParaboloidReflection::m_raster[1],
		mAmbientOcclusion->mSSAORaster };

	for (size_t i = 0; i < 5; i++)
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