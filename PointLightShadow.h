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

	RwRaster* mColorCube[30][6];
	RwRaster* mColorRaster[30];
	RwRaster* mDepthRaster;

	std::vector<CEntity*> m_renderableList[30];
	void AddObject(int i, CEntity* entity, float distance);
	void SectorList(CPtrList& ptrList);
	void ScanSectorList(int sectorX, int sectorY);

	void RenderEntities(PointLight* light, int i, int j);
	void Initialize();
	void Update();
};


extern PointLightShadow* PointShadow;