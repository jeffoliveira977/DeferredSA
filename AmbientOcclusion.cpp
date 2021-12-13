#include "AmbientOcclusion.h"
#include "Quad.h"
#include "CScene.h"
#include "imgui.h"

AmbientOcclusion::AmbientOcclusion()
{
	mSSAORaster = nullptr;
	mSSAOPixelShader = nullptr;
}

AmbientOcclusion::~AmbientOcclusion()
{}

void AmbientOcclusion::Initialize()
{
	mSSAORaster = RwD3D9RasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);

	mSSAOPixelShader = new PixelShader();
	mSSAOPixelShader->CreateFromBinary("SSAO_PS");

	//mSSAOCombinePixelShader = new PixelShader();
	//mSSAOCombinePixelShader->CreateFromBinary("SSAOCombine_PS");

}
#include "CTimer.h"
void AmbientOcclusion::Render()
{

	_rwD3D9SetPixelShaderConstant(8, &Scene.m_pRwCamera->farPlane, 1);
	static float ti = 0.0f;
	constexpr auto time_interval = 1.0f / (0.0005f) * 3000.0f;
	if(ti < time_interval)
		ti += CTimer::ms_fTimeStep;
	else
		ti = ti -
		time_interval + CTimer::ms_fTimeStep;

	mSSAOPixelShader->SetFloat("Time", ti);
	_rwD3D9SetPixelShader(mSSAOPixelShader->GetObject());
	RwD3D9SetRenderTarget(0, mSSAORaster);
	//RwD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0, 0);

	Quad::Render();
} 



void AmbientOcclusion::UpdateImgui()
{
	if(ImGui::BeginTabItem("Ambient Occlusion"))
	{
		/*ImGui::EndTabItem();
		ImGui::InputFloat("density", &density, 0.01, 0.1);
		ImGui::InputFloat("strength", &strength, 0.001, 0.01);

		ImGui::InputFloat("falloff", &falloff, 0.000001, 0.00001);
		ImGui::InputFloat("rad", &rad, 0.001, 0.01);
		ImGui::InputFloat("blur", &blur, 0.01);

		ImGui::InputFloat("dotRange", &dotRange, 0.01);

		ImGui::InputFloat("optimize", &optimize, 0.01);
		ImGui::InputFloat("fmRange", &fmRange, 0.01);
		ImGui::InputFloat("rnd_offset", &rnd_offset, 1.0);*/
	}
}
