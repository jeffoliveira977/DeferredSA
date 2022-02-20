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
#include "RenderableSphere.h"
#include "CCamera.h"
#include "LightManager.h"
#include "Texture.h"

RenderingStage gRenderState;
DeferredRendering *DeferredContext;
//RwTexture* gRandomNoise = 0;
//RwTexture* gFlashLight = 0;
Texture* gFlashLight;
Texture* gRandomNoise;

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

	//for(size_t i = 0; i < 4; i++)
	//{
	//	mGraphicsBuffer[i] = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	//	mGraphicsBuffer[i]->Initialize();
	//}

	mGraphicsBuffer[0] = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGraphicsBuffer[0]->Initialize();

	mGraphicsBuffer[1] = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGraphicsBuffer[1]->Initialize();

	mGraphicsBuffer[2] = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGraphicsBuffer[2]->Initialize();

	mGraphicsBuffer[3] = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGraphicsBuffer[3]->Initialize();

	mVolumetricClouds = unique_ptr<VolumetricClouds>(new VolumetricClouds());
	mVolumetricClouds->Initialize();

	mPostProcessing = unique_ptr<PostProcessing>(new PostProcessing());
	mPostProcessing->Initialize();

	mVolumetricLight = unique_ptr<VolumetricLight>(new VolumetricLight());
	mVolumetricLight->Initialize();

	mAmbientOcclusion = unique_ptr<AmbientOcclusion>(new AmbientOcclusion());
	mAmbientOcclusion->Initialize();

	gFlashLight = new Texture("DeferredSA/textures/flashlight2.png");
	gFlashLight->Initialize();

	gRandomNoise = new Texture("DeferredSA/textures/pcfnoise.bmp");
	gRandomNoise->Initialize();

	mShadowSampler.mMagFilter = D3DTEXF_POINT;
	mShadowSampler.mMinFilter = D3DTEXF_POINT;
	mShadowSampler.mMipFilter = D3DTEXF_POINT;
	mShadowSampler.mAddressU = D3DTADDRESS_BORDER;
	mShadowSampler.mAddressV = D3DTADDRESS_BORDER;
	mShadowSampler.mAddressW = D3DTADDRESS_BORDER;

	//mDefaultSampler.mAddressU = D3DTADDRESS_CLAMP;
	//mDefaultSampler.mAddressV = D3DTADDRESS_CLAMP;
	//mDefaultSampler.mAddressW = D3DTADDRESS_CLAMP;
}

XMMATRIX view, projection;
void DeferredRendering::Start()
{
	gRenderState = stageDeferred;

	mGraphicsBuffer[0]->SetAsRenderTarget(0);
	mGraphicsBuffer[1]->SetAsRenderTarget(1);
	mGraphicsBuffer[2]->SetAsRenderTarget(2);
	mGraphicsBuffer[3]->SetAsRenderTarget(3);

	rwD3D9Clear(nullptr, rwCAMERACLEARIMAGE);

	RwD3D9GetTransform(D3DTS_VIEW, &view);
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	_rwD3D9SetVertexShaderConstant(4, &view, 4); ;
	_rwD3D9SetVertexShaderConstant(8, &projection, 4);
	_rwD3D9SetPixelShaderConstant(4, &view, 4);

	_rwD3D9SetPixelShaderConstant(18, &(XMMatrixInverse(0, view* projection)), 4);
}

XMFLOAT4 corners[4];
void DeferredRendering::Stop()
{
	RwD3D9GetTransform(D3DTS_VIEW, &view);
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	_rwD3D9SetPixelShaderConstant(0, &XMMatrixInverse(nullptr, view), 4);
	_rwD3D9SetPixelShaderConstant(4, &projection, 4);

	XMVECTOR vFrustumCornersWS[8] =
	{
		{-1.0, 1.0, 0.0, 1.0}, // near top left
		{1.0, 1.0, 0.0, 1.0 }, // near top right
		{-1.0, -1.0, 0.0, 1.0}, // near bottom left
		{1.0, -1.0, 0.0, 1.0}, // near bottom right
		{-1.0, 1.0, 1.0, 1.0}, // far top left
		{1.0, 1.0, 1.0, 1.0}, // far top right
		{-1.0, -1.0, 1.0, 1.0}, // far bottom left
		{1.0, -1.0, 1.0, 1.0} // far bottom right
	};

	XMMATRIX invM = XMMatrixInverse(nullptr, view * projection);
	XMFLOAT4 Corners[8];
	for (int i = 0; i < 8; i++)
	{
		auto vertices = XMVector3TransformCoord(vFrustumCornersWS[i], invM);
		XMStoreFloat4(&Corners[i], vertices);
	}
	
	for (int i = 0; i < 4; ++i)
	{
		Corners[i].x = Corners[i + 4].x -= Corners[i].x;
		Corners[i].y = Corners[i + 4].y -= Corners[i].y;
		Corners[i].z = Corners[i + 4].z -= Corners[i].z;
	}

	 _rwD3D9SetVertexShaderConstant(0, Corners, 4);

	mGraphicsLight->SetAsTexture(&mDefaultSampler, 0);
	mGraphicsBuffer[0]->SetAsTexture(&mDefaultSampler, 1);
	mGraphicsBuffer[1]->SetAsTexture(&mDefaultSampler, 2);
	mGraphicsBuffer[2]->SetAsTexture(&mDefaultSampler, 3);
	mGraphicsBuffer[3]->SetAsTexture(&mDefaultSampler, 4);

	// We need to disable Z buffer
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	RenderLights();
	AtmosphericScattering();

	/*mAmbientOcclusion->Render();
	mVolumetricClouds->Render(mScreenRT.get());
	mVolumetricLight->Render(mScreenRT.get());*/

	_rwD3D9SetPixelShader(NULL);
	//_rwD3D9SetVertexShader(NULL);
}

void DeferredRendering::RenderPostProcessing()
{
	DefinedState();
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	mPostProcessing->RenderFXAA(mScreenRT.get());
	//mPostProcessing->RenderBloom(mScreenRT.get());
}

void DeferredRendering::RenderLights()
{
	ShaderContext->SetTimecyProps(8);
	ShaderContext->SetSunDirection(11);
	ShaderContext->SetFogParams(12);

	if (!CGame::currArea && CGameIdle::m_fShadowDNBalance < 1.0f)
	{
		CascadedShadowManagement->UpdateBuffer();

		for (size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
		{
			mShadowSampler.Apply(i + 5);
			rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 5);
		}

		_rwD3D9SetPixelShaderConstant(13, &CascadedShadowManagement->mConstantBuffer,
			sizeof(CascadedShadowManagement->mConstantBuffer) / sizeof(float[4]));

	}

	RwD3D9SetRenderTarget(0, mGraphicsLight->GetRaster());

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	mDirectLightPS->Apply();
	Quad::Render();

	float farClip[] = {
		Scene.m_pRwCamera->nearPlane,
	CTimeCycle::m_CurrentColours.m_fFarClip };
	_rwD3D9SetPixelShaderConstant(8, farClip, 1);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	mPointLightPS->Apply();
	// gLightManager.SortPointLights();
	CVector camPos = TheCamera.GetPosition();


	int count = 0;
	uint32_t maxLights = min((size_t)20, gLightManager.GetPointLightCount());
	for (int i = 0; i < maxLights; i++)
	{
		auto light = gLightManager.GetPointLightAt(i);
		auto radius = light->GetRadius();
		auto intensity = light->GetIntensity();

		CVector dx = CVector(light->GetPosition().x, light->GetPosition().y, light->GetPosition().z) - camPos;

		float drawShadow = 0.0;
		float visibleRadius = radius + 15.0f;
		if (!light->mDrawShadow)
			drawShadow = 1.0;

		mShadowSampler.Apply(5);
		_rwD3D9RWSetRasterStage(light->mShadowRaster, 5);

		gRandomNoise->Apply(&mDefaultSampler, 6);

		_rwD3D9SetPixelShaderConstant(9, &light->GetPosition(), 1);
		_rwD3D9SetPixelShaderConstant(10, &light->GetDirection(), 1);
		_rwD3D9SetPixelShaderConstant(11, &light->GetColor(), 1);
		_rwD3D9SetPixelShaderConstant(12, &radius, 1);
		_rwD3D9SetPixelShaderConstant(13, &intensity, 1);
		_rwD3D9SetPixelShaderConstant(14, &drawShadow, 1);

		Quad::Render();

	}	
	
	// PrintMessage("%d %d", maxLights, count);
	//gLightManager.SortSpotLights();
	mSpotLightPS->Apply();
	maxLights = min((size_t)30, gLightManager.GetSpotLightCount());

	static CPerfTimer shadowTimer("Shadow Time");
	shadowTimer.Start();

	gRandomNoise->Apply(&mDefaultSampler, 6);
	gFlashLight->Apply(&mDefaultSampler, 7);

	for (size_t i = 0; i < maxLights; i++)
	{
		auto light = gLightManager.GetSpotLightAt(i);

		mShadowSampler.Apply(5);
		_rwD3D9RWSetRasterStage(light->mColorRaster, 5);
	
		auto cosSpotAngle = cos(XMConvertToRadians(light->GetAngle()));
		auto spotexponent = light->mExponent / (1.0f - cosSpotAngle);

		struct
		{
			XMVECTOR LightPos;
			XMVECTOR LightDir;
			XMVECTOR LightColor;
			XMVECTOR Paramns;
			XMMATRIX Mat;
		} spotdata;

		spotdata.LightPos = XMVectorSetW(light->mPosition, light->mRadius);
		spotdata.LightDir = XMVectorSetW(light->mDirection, light->mIntensity);
		spotdata.LightColor = light->mColor;
		spotdata.Paramns = XMVectorSet(cosSpotAngle, spotexponent, static_cast<float>(light->mDrawShadow), static_cast<float>(light->mUsePattern));
		spotdata.Mat = light->mMatrix;

		_rwD3D9SetPixelShaderConstant(9, &spotdata, 8);
		Quad::Render();
	}

	shadowTimer.Stop();

	static float maxr=0.0f;
	maxr = max(maxr, shadowTimer.GetTimer());
	PrintMessage("%f %f", maxr, shadowTimer.GetTimer());

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
		mAmbientOcclusion->mDownSampled[1] };

	for (size_t i = 0; i < 5; i++)
	{
		mDefaultSampler.Apply(i + 4);
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

void DeferredRendering::UpdateTextures()
{
}