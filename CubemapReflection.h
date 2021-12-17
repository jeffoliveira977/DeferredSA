#pragma once
#include "CommonD.h"
#include "Frustum.h"
#include "RenderableReflectionObjects.h"

class CubemapReflection
{
public:
	static int m_size;
	static XMMATRIX m_projectionMatrix;
	static XMMATRIX m_viewMatrix[6];
	static Math::Frustum m_frustum[6];
	static std::vector<CEntity*> m_renderableList[6];
	static RwCamera* m_camera;
	static RwRaster* m_raster;
	static RwRaster* m_cubeRaster;
	static RwRaster* m_depthRaster;
	static RwFrame* m_frame;
	static std::unordered_map<CEntity*, bool> mObjectsCulled[6];

	static void Initialize();
	static void Release();
	static void AddObject(int i, CEntity* entity, float distance);
	static void SectorList(CPtrList& ptrList);
	static void ScanSectorList(int sectorX, int sectorY);
	static void Update();

	static void RenderScene();
};

