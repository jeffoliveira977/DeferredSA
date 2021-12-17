#pragma once

#include "CommonD.h"
#include "RwMath.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>

class SpotlightShadow
{
public:
	XMVECTOR m_spotPos;
	XMVECTOR m_spotDir;
	float m_spotAngle;
	float m_spotRadius;
	float m_nShadowSize;
	RwRaster* mColorRaster[30];
	RwRaster* mDepthRaster;
	XMMATRIX m_shadowMatrix[30];
	RwCamera* mCamera;

	SpotlightShadow();
	~SpotlightShadow();

	void Initialize();
	void Update();

};


extern SpotlightShadow *SpotShadow;
