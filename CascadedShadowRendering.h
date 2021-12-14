#pragma once

#include "ShadowFrustum.h"
class CascadedShadowRendering : public ShadowFrustum
{
public:
	CascadedShadowRendering();
	~CascadedShadowRendering();

	struct ShadowData
	{
		XMVECTOR mDistanceCoefficients;
		XMVECTOR mShadowParam;
		XMVECTOR mShadowBias;
		XMMATRIX mShadowMatrix[4];
	};

	ShadowData mConstantBuffer;
	RwCamera* mCamera;
	RwRaster* mColorRaster[4];
	RwRaster* mDepthRaster;

	void Initialize();
	void Update();
	void SetParamsBuffer();
	void UpdateImgui();
	void UpdateTextures();
};
extern CascadedShadowRendering *CascadedShadowManagement;