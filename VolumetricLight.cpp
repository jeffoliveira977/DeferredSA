#include "VolumetricLight.h"
#include "ShaderManager.h"
#include "CascadedShadowRendering.h"
#include "Quad.h"
#include "imgui.h"

float VolumetricLightParam[3];

VolumetricLight::VolumetricLight()
{
	m_volumetricLight = nullptr;
	m_volumetricLight = nullptr;
}

VolumetricLight::~VolumetricLight()
{}

void VolumetricLight::Initialize()
{
	m_volumetricLight = RwD3D9RasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, D3DFMT_A8R8G8B8, rwRASTERTYPECAMERATEXTURE);
	m_screenRaster = RwD3D9RasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);

	PS_VolumetricLight = RwCreateCompiledPixelShader("VolumetricLight");
	PS_VolumetricLightCombine = RwCreateCompiledPixelShader("VolumetricLightCombine");

	VolumetricLightParam[0] = 97.5;
	VolumetricLightParam[1] = 0.74000001;
	VolumetricLightParam[2] = 0.107;
}

void VolumetricLight::Render()
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
	RwD3D9SetRenderTarget(0, m_volumetricLight);
	DrawScreenQuad();

	IDirect3DSurface9* screenSurface;
	auto screenExt = RASTEREXTFROMRASTER(m_screenRaster);
	screenExt->texture->GetSurfaceLevel(0, &screenSurface);
	RwD3DDevice->StretchRect(RwD3D9RenderSurface, NULL, screenSurface, NULL, D3DTEXF_NONE);

	screenSurface->Release();
	rwD3D9RWSetRasterStage(m_screenRaster, 4);
	rwD3D9RWSetRasterStage(m_volumetricLight, 5);

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

void VolumetricLight::UpdateTextures()
{
	int width, height;
	width = RsGlobal.maximumWidth;
	height = RsGlobal.maximumHeight;

	//if(width != m_screenRaster->width || height != m_screenRaster->height)
	//{
		m_screenRaster->width = width;
		m_screenRaster->height = height;

		m_volumetricLight->width = width;
		m_volumetricLight->height = height;

	//}
}
