#pragma once
#include "CommonD.h"
#include "RenderableFrustum.h"

class RenderableScene
{
public:
	static void InitGraphicsBuffer();
	static void Render();
	static void SetRenderList(std::vector<CEntity*>list);
	static RwFrame* m_frame;
	static RwRaster* m_raster;
	static RwRaster* m_depthRaster;
	static RwCamera* m_camera;
	static std::vector<CEntity*> m_list;
	static RenderableFrustum *m_frustumRenderable;
	static RenderCallback m_renderCallback;

};

