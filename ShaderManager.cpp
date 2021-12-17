#include "ShaderManager.h"
#include "RenderWare.h"
#include "CTimeCycle.h"
#include "CScene.h"
#include <algorithm>
#include <tuple>
#include <vector>
ShaderManager* ShaderContext;

std::vector<char*> ShaderList;
void ShaderManager::LoadShaders()
{

}

void ShaderManager::SetTimecyProps(int idx)
{
	//Update();
	_rwD3D9SetPixelShaderConstant(idx, &m_skyTop, 1); idx++;
	_rwD3D9SetPixelShaderConstant(idx, &m_skyBottom, 1); idx++;
	_rwD3D9SetPixelShaderConstant(idx, &m_sunColor, 1); idx++;
	_rwD3D9SetPixelShaderConstant(idx, sunDirs, 1); idx++;
	_rwD3D9SetPixelShaderConstant(idx, m_planeData, 1);
}

void ShaderManager::SetSkyColor(int idx)
{
	/*m_skyTop = {
				static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopRed) / 255.0f,
				static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen) / 255.0f,
				static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue) / 255.0f};

	m_skyBottom = {
	   static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed) / 255.0f,
	   static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen) / 255.0f,
	   static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue) / 255.0f};*/

	_rwD3D9SetPixelShaderConstant(idx, &m_skyTop, 1); idx++;
	_rwD3D9SetPixelShaderConstant(idx, &m_skyBottom, 1);
}

void ShaderManager::SetSunDirection(int idx)
{
	/*sunDirs = (CVector*)0xB7CA50;
	int sunDirIndex = *(int*)0xB79FD0;
	sunDirs[sunDirIndex];*/
	_rwD3D9SetPixelShaderConstant(idx, sunDirs, 1);
}

void ShaderManager::SetViewMatrix(int idx, bool vs)
{
	RwMatrix view;
	RwD3D9GetTransform(D3DTS_VIEW, &view);

	if(vs)
		_rwD3D9SetVertexShaderConstant(idx, &view, 4);
	else
		_rwD3D9SetPixelShaderConstant(idx, &view, 4);
}

void ShaderManager::SetInverseViewMatrix(int idx, bool vs)
{
	XMMATRIX view, inverseView;

	RwD3D9GetTransform(D3DTS_VIEW, &view);
	inverseView = XMMatrixInverse(NULL, view);

	if(vs)
		_rwD3D9SetVertexShaderConstant(idx, &inverseView, 4);
	else
		_rwD3D9SetPixelShaderConstant(idx, &inverseView, 4);
}

void ShaderManager::SetInverseProjectionMatrix(int idx, bool vs)
{
	XMMATRIX projection, inverseProjection;

	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);
	inverseProjection = XMMatrixInverse(NULL, projection);

	if(vs)
		_rwD3D9SetVertexShaderConstant(idx, &inverseProjection, 4);
	else
		_rwD3D9SetPixelShaderConstant(idx, &inverseProjection, 4);
}

void ShaderManager::SetProjectionMatrix(int idx, bool vs)
{
	RwMatrix projection;
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	if(vs)
		_rwD3D9SetVertexShaderConstant(idx, &projection, 4);
	else
		_rwD3D9SetPixelShaderConstant(idx, &projection, 4);
}

void ShaderManager::SetViewProjectionMatrix(int idx, bool vs)
{
	RwMatrix view, projection;
	RwD3D9GetTransform(D3DTS_VIEW, &view);
	RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

	if(vs)
	{
		_rwD3D9SetVertexShaderConstant(idx, &view, 4); idx += 4;
		_rwD3D9SetVertexShaderConstant(idx, &projection, 4);
	}
	else
	{
		_rwD3D9SetPixelShaderConstant(idx, &view, 4); idx += 4;
		_rwD3D9SetPixelShaderConstant(idx, &projection, 4);
	}
}

void ShaderManager::SetInverseViewProjectionMatrix()
{}

void ShaderManager::SetSunColor(int idx)
{
	/*m_sunColor = {
			 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreRed) / 255.0f,
			 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreGreen) / 255.0f,
			 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreBlue) / 255.0f};*/
	_rwD3D9SetPixelShaderConstant(idx, &m_sunColor, 1);
}
#include "CGameIdle.h"
void ShaderManager::SetFogParams(int idx)
{
	/*m_planeData[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	m_planeData[1] = Scene.m_pRwCamera->farPlane;
	m_planeData[3] = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);*/
	_rwD3D9SetPixelShaderConstant(idx, m_planeData, 1);
}

void ShaderManager::Update()
{
	m_skyTop = {
			 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopRed) / 255.0f,
			 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen) / 255.0f,
			 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue) / 255.0f};

	m_skyBottom = {
	   static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed) / 255.0f,
	   static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen) / 255.0f,
	   static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue) / 255.0f};

	m_sunColor = {
		  static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreRed) / 255.0f,
		  static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreGreen) / 255.0f,
		  static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreBlue) / 255.0f};

	sunDirs = (CVector*)0xB7CA50;
	int sunDirIndex = *(int*)0xB79FD0;
	sunDirs[sunDirIndex];

	m_planeData[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	m_planeData[1] = Scene.m_pRwCamera->farPlane;
	m_planeData[2] = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
	m_planeData[3] = CTimeCycle::m_CurrentColours.m_fFarClip - CTimeCycle::m_CurrentColours.m_fFogStart;
}