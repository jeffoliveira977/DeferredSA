#pragma once
#include "CommonD.h"
#include "Frustum.h"
#include "RenderableReflectionObjects.h"
class EnvironmentMapping 
{
public:
	static RenderCallback m_renderCallback;
	static Math::Frustum m_frustum[6];
	static RwMatrix m_ltm[6];
	static RwRaster* m_cubeRaster;
	static RwRaster* m_paraboloidRaster[2];
	static RwRaster* m_sphericalRaster;
	static RwRaster* m_depthRaster;
	static RwFrame* m_envFrame;
	static RwCamera* m_envCamera;
	static int m_weight;
	static float m_direction;
	static D3DXMATRIX m_paraboloidBasis;
	static XMMATRIX m_projectionMatrix;
	static XMMATRIX m_viewMatrix[6];

	static std::vector<CEntity*> m_renderableList[6];

	static void AddObject(int i, CEntity* entity, float distance);
	static void SectorList(CPtrList& ptrList);
	static void SetRenderCallback(RenderCallback callback);
	static void InitializeGraphics();
	static void PlaneMap();
	static void SphericalMap();

	static void UpdateCubeMap();
	static void DualParaboloidMap();
	static void CubeMap();
};


