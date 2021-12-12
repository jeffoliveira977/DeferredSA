#include "VolumetricClouds.h"
#include "CommonD.h"
#include "ShaderManager.h"
#include "CTimer.h"
#include "Quad.h"

VolumetricClouds::VolumetricClouds()
{
	mPixelShader = nullptr;
	mCombinePixelShader = nullptr;
	mVolumetricCloudRaster = nullptr;
}

VolumetricClouds::~VolumetricClouds()
{
	delete mPixelShader;
	delete mCombinePixelShader;
	RwRasterDestroy(mVolumetricCloudRaster);
}

void VolumetricClouds::Initialize()
{
	mPixelShader = new PixelShader();
	mPixelShader->CreateFromBinary("VolumetricClouds");

	mCombinePixelShader = new PixelShader();
	mCombinePixelShader->CreateFromBinary("VolumetricCloudsCombine");

	mVolumetricCloudRaster = RwD3D9RasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);
}

void VolumetricClouds::Render(RwRaster* screenSource)
{
	ShaderContext->SetInverseViewMatrix(0);
	ShaderContext->SetProjectionMatrix(4);
	ShaderContext->SetSunColor(8);
	ShaderContext->SetSunDirection(9);

	ShaderContext->SetFogParams(12);

	static float ti = 0.0f;
	constexpr auto time_interval = 1.0f / (0.0005f) * 3000.0f;
	if(ti < time_interval)
		ti += CTimer::ms_fTimeStep;
	else
		ti = ti -
		time_interval + CTimer::ms_fTimeStep;

	_rwD3D9SetPixelShaderConstant(13, &ti, 1);

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

	//_rwD3D9SetPixelShader(mVolumetricCloudsPixelShader);
	//rwD3D9RWSetRasterStage(m_graphicsBuffer[1], 2);
	//RwD3D9SetRenderTarget(0, mVolumetricCloudRaster);
	//DrawScreenQuad();

	_rwD3D9SetPixelShader(mPixelShader->GetShader());
	rwD3D9RWSetRasterStage(screenSource, 4);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	Quad::Render();
}
