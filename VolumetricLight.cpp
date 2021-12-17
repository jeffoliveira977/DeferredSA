#include "VolumetricLight.h"
#include "ShaderManager.h"
#include "CascadedShadowRendering.h"
#include "Quad.h"
#include "imgui.h"

float VolumetricLightParam[3];

VolumetricLight::VolumetricLight()
{
	mVolumetricLightRaster = nullptr;
}

VolumetricLight::~VolumetricLight()
{}

void VolumetricLight::Initialize()
{
	mVolumetricLightRaster = new RenderTarget(D3DFMT_A16B16G16R16F);
	mVolumetricLightRaster->Initialize();

	PS_VolumetricLight = RwCreateCompiledPixelShader("VolumetricLight");
	PS_VolumetricLightCombine = RwCreateCompiledPixelShader("VolumetricLightCombine");

	VolumetricLightParam[0] = 97.5;
	VolumetricLightParam[1] = 0.74000001;
	VolumetricLightParam[2] = 0.107;
}

void VolumetricLight::Render(RenderTarget* screenSource)
{
	ShaderContext->SetTimecyProps(8);

	_rwD3D9SetPixelShaderConstant(13, VolumetricLightParam, 1);

	for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
	{
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 4);
	}

	_rwD3D9SetPixelShaderConstant(14, &CascadedShadowManagement->mConstantBuffer,
								  sizeof(CascadedShadowManagement->mConstantBuffer) / sizeof(XMVECTOR));

	_rwD3D9SetPixelShader(PS_VolumetricLight);
	_rwD3D9SetVertexShader(0);
	RwD3D9SetRenderTarget(0, mVolumetricLightRaster->GetRaster());
	DrawScreenQuad();

	screenSource->CopyFromSurface(nullptr);

	rwD3D9RWSetRasterStage(screenSource->GetRaster(), 4);
	rwD3D9RWSetRasterStage(mVolumetricLightRaster->GetRaster(), 5);

	_rwD3D9SetPixelShader(PS_VolumetricLightCombine);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	DrawScreenQuad();
}

void VolumetricLight::UpdateImgui()
{
	if(ImGui::BeginTabItem("Deferred"))
	{
		ImGui::EndTabItem();
		ImGui::InputFloat("RaymarchingDistance", &VolumetricLightParam[0], 0.1, 1.0, "%.1f");
		ImGui::InputFloat("SunlightBlendOffset", &VolumetricLightParam[1], 0.01, 0.1, "%.6f");
		ImGui::InputFloat("SunlightIntensity", &VolumetricLightParam[2], 0.01, 0.1, "%.3f");
	}
}