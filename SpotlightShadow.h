#pragma once

#include "CommonD.h"
#include "RwMath.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "Frustum.h"
class SpotlightShadow
{
public:
	XMVECTOR m_spotPos;
	XMVECTOR m_spotDir;
	float m_spotAngle;
	float m_spotRadius;
	int m_nShadowSize;
	RwRaster* mColorRaster[30];
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
	SpotlightShadow();
	~SpotlightShadow();
	void SpotlightShadow::RenderEntities(int i);
	void Initialize();
	void Update();

};


extern SpotlightShadow *SpotShadow;
