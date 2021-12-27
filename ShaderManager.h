#pragma once
#include "CommonD.h"

class ShaderManager
{
public:
	CVector m_skyTop;
	CVector m_skyBottom;
	CVector m_sunColor;
	float m_planeData[4];
	CVector sunDirs;



	void LoadShaders();
	void SetTimecyProps(int idx);
	void SetSunColor(int idx);
	void SetFogParams(int idx);
	void SetSkyColor(int idx);
	void SetSunDirection(int idx);
	void SetViewMatrix(int idx, bool vs = false);
	void SetInverseViewMatrix(int idx, bool vs = false);
	void SetInverseProjectionMatrix(int idx, bool vs = false);
	void SetProjectionMatrix(int idx, bool vs = false);
	void SetViewProjectionMatrix(int idx, bool vs = false);
	void SetInverseViewProjectionMatrix(int idx, bool vs);
	void Update();
};

extern ShaderManager* ShaderContext;
