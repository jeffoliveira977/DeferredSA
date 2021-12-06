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
	RwRaster* m_shadowColorRaster[30];
	RwRaster* m_shadowDepthRaster;
	XMMATRIX m_shadowMatrix[30];
	RwCamera* m_pShadowCamera;

	SpotlightShadow();
	~SpotlightShadow();

	void initGraphics();
	void Update();

};


extern SpotlightShadow *SpotShadow;
