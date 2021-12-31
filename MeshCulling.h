#pragma once
#include "CommonD.h"
#include "Frustum.h"
class MeshCulling
{

public:

	void AddObject(CEntity*entity);
	static bool InFrustum(CEntity* entity, Math::Frustum frustum);
	static bool Render(vector<CEntity*>list, Math::Frustum frustum);
};

