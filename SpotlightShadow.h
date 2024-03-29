#pragma once

#include "CommonD.h"
#include "RwMath.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "Frustum.h"
#include "SpotLight.h"
class SpotlightShadow
{
public:
	int m_nShadowSize;
	RwRaster* mColorRaster[30];
	RwRaster* mDepthRaster;

	std::vector<CEntity*> m_renderableList;
	void AddObject(CEntity* entity);
	SpotlightShadow();
	~SpotlightShadow();
	void RenderEntities(SpotLight*light, int i);
	void Initialize();
	void Update();
};


extern SpotlightShadow *SpotShadow;
