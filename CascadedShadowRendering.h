#pragma once

#include "ShadowFrustum.h"
class CascadedShadowRendering : public ShadowFrustum
{
public:
	CascadedShadowRendering();
	~CascadedShadowRendering();

	struct ShadowData
	{
		XMFLOAT4 mDistanceCoefficients;
		XMFLOAT4 mShadowParam;
		XMFLOAT4 mShadowBias;
		XMMATRIX mShadowMatrix[4];
	};

	ShadowData mConstantBuffer;
	RwCamera* mCamera;
	RwRaster* mColorRaster[4];
	RwRaster* mDepthRaster;

	void Initialize();
	void Update();
	void UpdateBuffer();
	void UpdateImgui();
	void UpdateTextures();
};
extern CascadedShadowRendering *CascadedShadowManagement;