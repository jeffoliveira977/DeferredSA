#pragma once
#include "CommonD.h"
#include "Frustum.h"

class DualParaboloidReflection
{
public:
	static int m_size;
	static XMMATRIX m_projectionMatrix;
	static XMMATRIX m_viewMatrix[2];
	static Math::Frustum m_frustum[2];
	static std::vector<CEntity*> m_renderableList[2];
	static RwCamera* m_camera;
	static RwRaster* m_raster[2];
	static RwRaster* m_depthRaster;
	static RwFrame* m_frame;
	static float m_direction;

	static void Initialize();
	static void Release();
	static void AddObject(int i, CEntity* entity, float distance);
	static void SectorList(CPtrList& ptrList);
	static void ScanSectorList(int sectorX, int sectorY);
	static void Update();
	static void RenderEntities(int i);
	static void RenderScene();
};

