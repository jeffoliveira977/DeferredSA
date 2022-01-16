#pragma once
#include "CommonD.h"
#include "Frustum.h"

enum MeshRenderList : int8_t
{
	BUILDING_SCAN=1,
	PED_SCAN,
	VEHICLE_SCAN,
	OBJECT_SCAN
};
class MeshCulling
{

public:

	void AddObject(CEntity*entity);
	static bool InFrustum(CEntity* entity, Math::Frustum frustum);
	static bool Render(vector<CEntity*>list, Math::Frustum frustum);

	static bool GetMeshRenderList(int8_t scan, std::vector<CEntity*>& meshList);
};

