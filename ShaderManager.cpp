#include "ShaderManager.h"
#include "RenderWare.h"
#include "CTimeCycle.h"
#include "CScene.h"
#include <algorithm>
#include <tuple>
#include <vector>
#include "CGameIdle.h"
#include "PixelShaderConstant.h"
#include "VertexShaderConstant.h"
ShaderManager* ShaderContext;

void ShaderManager::SetTimecyProps(RwUInt32 index)
{
	PixelShaderConstant::SetVector(index+0, &mSkyTopColor);
	PixelShaderConstant::SetVector(index+1, &mSkytBottomColor);
	PixelShaderConstant::SetVector(index+2, &mSunColor);
	PixelShaderConstant::SetVector(index+3, sunDirs);
	PixelShaderConstant::SetVector(index+4, mSettings);
}

void ShaderManager::SetSkyColor(RwUInt32 index)
{
	PixelShaderConstant::SetVector(index+0, &mSkyTopColor);
	PixelShaderConstant::SetVector(index+1, &mSkytBottomColor);
}

void ShaderManager::SetSunDirection(RwUInt32 index)
{
	PixelShaderConstant::SetVector(index, sunDirs);
}

void ShaderManager::SetViewMatrix(RwUInt32 index, bool vs)
{
	XMMATRIX view;
	RwD3D9GetTransform(D3DTS_VIEW, &view);

	if(vs)
		VertexShaderConstant::SetMatrix(index, view);
	else
		PixelShaderConstant::SetMatrix(index, view);
}

void ShaderManager::SetInverseViewMatrix(RwUInt32 index, bool vs)
{
	XMMATRIX view, inverseView;

	RwD3D9GetTransform(D3DTS_VIEW, &view);
	inverseView = XMMatrixInverse(NULL, view);

	if(vs)
		VertexShaderConstant::SetMatrix(index, inverseView);
	else
		PixelShaderConstant::SetMatrix(index, inverseView);
}

void ShaderManager::SetProjectionMatrix(RwUInt32 index, bool vs)
{
	XMMATRIX projection;
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	if(vs)
		VertexShaderConstant::SetMatrix(index, projection);
	else
		PixelShaderConstant::SetMatrix(index, &projection);
}

void ShaderManager::SetViewProjectionMatrix(RwUInt32 startIndex, bool vs)
{
	XMMATRIX view, projection;
	RwD3D9GetTransform(D3DTS_VIEW, &view);
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	if(vs)
	{
		VertexShaderConstant::SetMatrix(startIndex, view);
		VertexShaderConstant::SetMatrix(startIndex + 4, projection);
	}
	else
	{
		VertexShaderConstant::SetMatrix(startIndex, view);
		VertexShaderConstant::SetMatrix(startIndex + 4, projection);
	}
}

void ShaderManager::SetViewProjectionMatrix(RwUInt32 index, XMMATRIX view, XMMATRIX projection)
{
	RwD3D9SetTransform(D3DTS_VIEW, &view);
	RwD3D9SetTransform(D3DTS_PROJECTION, &projection);

	VertexShaderConstant::SetMatrix(index, view);
	VertexShaderConstant::SetMatrix(index + 4, projection);
}

void ShaderManager::SetSunColor(RwUInt32 index)
{
	VertexShaderConstant::SetVector(index, &mSunColor);
}

void ShaderManager::SetFogParams(RwUInt32 index)
{
	VertexShaderConstant::SetVector(index, mSettings);
}

void ShaderManager::Update()
{
	auto colourSet = CTimeCycle::m_CurrentColours;

	mSkyTopColor = {static_cast<float>(colourSet.m_nSkyTopRed) / 255.0f,
			 static_cast<float>(colourSet.m_nSkyTopGreen) / 255.0f,
			 static_cast<float>(colourSet.m_nSkyTopBlue) / 255.0f};

	mSkytBottomColor = {static_cast<float>(colourSet.m_nSkyBottomRed) / 255.0f,
	   static_cast<float>(colourSet.m_nSkyBottomGreen) / 255.0f,
	   static_cast<float>(colourSet.m_nSkyBottomBlue) / 255.0f};

	mSunColor = {static_cast<float>(colourSet.m_nSunCoreRed) / 255.0f,
		  static_cast<float>(colourSet.m_nSunCoreGreen) / 255.0f,
		  static_cast<float>(colourSet.m_nSunCoreBlue) / 255.0f};

	sunDirs = (CVector*)0xB7CA50;
	int sunDirIndex = *(int*)0xB79FD0;
	sunDirs[sunDirIndex];

	mSettings[0] = colourSet.m_fFogStart;
	mSettings[1] = Scene.m_pRwCamera->farPlane;
	mSettings[2] = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
	mSettings[3] = colourSet.m_fFarClip - colourSet.m_fFogStart;
}