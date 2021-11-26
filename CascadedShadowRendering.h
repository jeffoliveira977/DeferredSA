#pragma once

#include "ShadowFrustum.h"
class CascadedShadowRendering : public ShadowFrustum
{
public:
	CascadedShadowRendering();
	~CascadedShadowRendering();

	struct ShadowData
	{
		XMVECTOR DistanceCoefficients;
		XMVECTOR ShadowParam;
		XMVECTOR ShadowBias;
		XMMATRIX mShadowMatrix[4];
	};

	ShadowData m_shadowBuffer;
	RwCamera* m_pShadowCamera[4];
	RwRaster* m_shadowColorRaster[4];
	RwRaster* m_shadowDepthRaster;

	RwMatrix viewMatrix[4];
	RwV2d ViewWindow[4];
	RwV2d ViewOffset[4];
	Plane Planes[4][6];

	void imguiParameters();
	void initGraphics();
	void Update();
	void RenderShadowToBuffer(int cascade, void(*render)(int cascade));
	void SetParamsBuffer();
};
extern CascadedShadowRendering *CascadedShadowManagement;