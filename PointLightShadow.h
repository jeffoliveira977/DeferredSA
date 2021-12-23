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
	float m_nShadowSize;
	RwRaster* mColorRaster[30];
	RwRaster* mDualParaboloidFront[30];
	RwRaster* mDualParaboloidBack[30];
	RwRaster* mDepthRaster;
	XMMATRIX m_shadowMatrix[30];
	XMMATRIX m_projectionMatrix[30];
	XMMATRIX m_viewMatrix[30];
	RwCamera* mCamera;
	Math::Frustum m_frustum[30];
	std::vector<CEntity*> m_renderableList[30];
	void AddObject(int i, CEntity* entity, float distance);
	void SectorList(CPtrList& ptrList);
	void ScanSectorList(int sectorX, int sectorY);

	void RenderEntities(PointLight light, int i, int j);
	void Initialize();
	void Update();
};


extern PointLightShadow* PointShadow;