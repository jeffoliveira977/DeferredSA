#pragma once
#include "CommonD.h"
#include "Frustum.h"

#include "PointLight.h"

class PointLightShadow
{
public:
	PointLightShadow();
	~PointLightShadow();

	XMVECTOR m_spotPos;
	XMVECTOR m_spotDir;
	float m_spotAngle;
	float m_spotRadius;
	int m_nShadowSize;

	 RwRaster* mColorRaster[30];
	 RwRaster* mColorCube[30];
	RwRaster* mDepthRaster;

	std::vector<CEntity*> m_renderableList;
	void AddObject(CEntity* entity);

	void RenderEntities(PointLight* light, int i, int j);
	void Initialize();
	void Update();
};


extern PointLightShadow* PointShadow;