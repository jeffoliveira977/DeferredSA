#pragma once
#include "CommonD.h"

class EnvironmentMapping
{
public:
	static RenderCallback m_renderCallback;

	static RwRaster* m_cubeRaster;
	static RwRaster* m_paraboloidRaster[2];
	static RwRaster* m_sphericalRaster;
	static RwRaster* m_depthRaster;
	static RwFrame* m_envFrame;
	static RwCamera* m_envCamera;
	static int m_weight;
	static float m_direction;
	static D3DXMATRIX m_paraboloidBasis;
	static void EnvironmentMapping::RenderOneFace(
		void (*renderCB)(),
		int id, float angleA, RwV3d axisA, float angleB, RwV3d axisB, RwV3d camPos);
	static void SetRenderCallback(RenderCallback callback);
	static void InitializeGraphics();
	static void PlaneMap();
	static void SphericalMap();
	static void DualParaboloidMap();
	static void CubeMap();
};


