#pragma once
#include "game_sa\CVector.h"
#include "game_sa\CMatrix.h"
#include "game_sa\CEntity.h"
#include "game_sa\CVehicleModelInfo.h"
#include "CommonD.h"

// Default SA light type for our light
enum eLightMode
{
	LIGHT_MODE_POINT,
	LIGHT_MODE_DIRECTIONAL,
	LIGHT_MODE_SHADOW,
	LIGHT_MODE_GLOBAL = 4
};

// Our type
enum eLightType
{
	LIGHT_TYPE_OMNI,
	LIGHT_TYPE_SPOT
};


struct LightData
{
	CVector pos;
	float radius;
	CVector dir;
	float type;
	CVector color;
	float pad;
};
#include "CPointLights.h"
class Lights
{
	
	static LightData m_aLights[1024];

	static void AddLight(LightData& light);
public:
	static std::vector<LightData> LightList;
	static int m_nLightCount;
	static void SortByDistance(const RwV3d& from);
	static void ClearLights();
	static LightData* Buffer();
	static void AddOmniLight(ePointLightType, CVector, CVector, float, float, float, float, unsigned char, bool, CEntity*);
	static void __thiscall AddSpotLight(CVehicle* vehicle, int a, CMatrix* matrix, bool isRight);
	static void __thiscall DoHeadLightReflection(CVehicle* vechile, CMatrix* matrix, unsigned int flags, unsigned char left, unsigned char right);
	static void __thiscall StoreShadowForVehicle(CVehicle* vehicle, int32_t vehShadowType);
	static void __cdecl StoreCarLightShadow(CVehicle* vehicle, int32_t id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, int8_t red, int8_t green, int8_t blue, float maxViewAngle);

	static void Patch();
};

