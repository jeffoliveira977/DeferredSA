#include "VolumetricClouds.h"
#include "CommonD.h"
#include "ShaderManager.h"
#include "CTimer.h"
#include "Quad.h"
#include "ShaderConstant.h"

VolumetricClouds::VolumetricClouds()
{
	mPixelShader = nullptr;
	mCombinePixelShader = nullptr;
}

VolumetricClouds::~VolumetricClouds()
{
	delete mPixelShader;
	delete mCombinePixelShader;
}

void VolumetricClouds::Initialize()
{
	mPixelShader = new PixelShader();
	mPixelShader->CreateFromBinary("VolumetricClouds");

	//mCombinePixelShader = new PixelShader();
	//mCombinePixelShader->CreateFromBinary("VolumetricCloudsCombine");

}

#include "CTimeCycle.h"
#include "CScene.h"
void VolumetricClouds::Render(RenderTarget* screenSource)
{
	/*ShaderContext->SetInverseViewMatrix(0);
	ShaderContext->SetProjectionMatrix(4);
	ShaderContext->SetSunColor(8);
	ShaderContext->SetSunDirection(9);
	ShaderContext->SetFogParams(12);*/

	XMMATRIX view, projection;

	RwD3D9GetTransform(D3DTS_VIEW, &view);
	view = XMMatrixInverse(0, view);
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	CVector m_sunColor = {
		  static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreRed) / 255.0f,
		  static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreGreen) / 255.0f,
		  static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreBlue) / 255.0f};

	CVector* sunDirs = (CVector*)0xB7CA50;
	int sunDirIndex = *(int*)0xB79FD0;
	sunDirs[sunDirIndex];
	float m_planeData[2];

	m_planeData[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	m_planeData[1] = Scene.m_pRwCamera->farPlane;
	
	/*mPixelShader->SetMatrix("mViewInv", &view);
	mPixelShader->SetMatrix("Projection", &projection);*/
	mPixelShader->SetValue("vSunColor", &m_sunColor, sizeof(m_sunColor));
	mPixelShader->SetValue("SunDirection", &sunDirs, sizeof(CVector));
	mPixelShader->SetValue("fogData", &m_planeData, sizeof(m_planeData));
	
	static float ti = 0.0f;
	constexpr auto time_interval = 1.0f / (0.0005f) * 3000.0f;
	if(ti < time_interval)
		ti += CTimer::ms_fTimeStep;
	else
		ti = ti -
		time_interval + CTimer::ms_fTimeStep;

	//_rwD3D9SetPixelShaderConstant(13, &ti, 1);
	mPixelShader->SetFloat("Time", ti);

	rwD3D9SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	rwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	rwD3D9SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	rwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	rwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	rwD3D9SetSamplerState(2, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

	rwD3D9SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	rwD3D9SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	rwD3D9SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	rwD3D9SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	rwD3D9SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	rwD3D9SetSamplerState(4, D3DSAMP_MAXMIPLEVEL, 16);

	_rwD3D9SetPixelShader(mPixelShader->GetObject());

	screenSource->CopyFromSurface(nullptr);
	rwD3D9RWSetRasterStage(screenSource->GetRaster(), 4);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	Quad::Render();
}