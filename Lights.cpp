#include "Lights.h"
#include "plugin.h"
#include <game_sa\CVehicleModelInfo.h>
#include <game_sa\CModelInfo.h>

using namespace plugin;


std::vector<LightData>  Lights::LightList;
int Lights::m_nLightCount = 0;
LightData Lights::m_aLights[1024];

void Lights::Patch()
{
	patch::Nop(0x6E27E6, 5); // fix
	patch::RedirectJump(0x6E0E20, AddSpotLight);
	patch::RedirectJump(0x7000E0, AddOmniLight);
}
float rwV3D_Dist(const CVector& a, const CVector& b)
{
	float
		dX = b.x - a.x,
		dY = b.y - a.y,
		dZ = b.z - a.z;

	return sqrt(dX * dX + dY * dY + dZ * dZ);
}

void Lights::AddLight(LightData &light)
{
	if(m_nLightCount < 1024)
	{
		m_aLights[m_nLightCount] = light; 
		m_nLightCount++;
	}
}

void Lights::SortByDistance(const RwV3d& from)
{
	std::sort(&m_aLights[0], &m_aLights[1023], [&](const LightData& a, const LightData& b)
	{
		return (rwV3D_Dist(a.pos, from) < rwV3D_Dist(b.pos, from));
	});
}

void Lights::ClearLights()
{
	LightList.clear();
	m_nLightCount = 0;
}

LightData* Lights::Buffer()
{
	return &m_aLights[0];
}

#include "DeferredRenderer.h"
#include "CPointLights.h"
#include "CCamera.h"

void Lights::AddOmniLight(ePointLightType defaultType, CVector pos, CVector dir, float radius, float red, float green, float blue, unsigned char fogType,
						   bool generateExtraShadows, CEntity *entityAffected)
{
	if(gRenderState == stageSphereMap)
		return;
	
	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = radius + 15.0f;
	CVector dx = pos - camPos;
	//if(dx.x >= visibleRadius || dx.x <= -visibleRadius)
	//{
	//	return;
	//}
	//if(dx.y >= visibleRadius || dx.y <= -visibleRadius)
	//{
	//	return;
	//}
	/*if(CPointLights::NumLights >= 32)
	{
		return;
	}*/
	//if(dx.Magnitude() >= visibleRadius)
	//{
	//	return;
	//}
	size_t id = CPointLights::NumLights;
	//CPointLights::aLights[id].m_nType = defaultType;
	//CPointLights::aLights[id].m_nFogType = fogType;
	//CPointLights::aLights[id].m_vecPosn = pos;
	//CPointLights::aLights[id].m_vecDirection = dir;
	//CPointLights::aLights[id].m_fRange = radius;
	//CPointLights::aLights[id].m_bGenerateShadows = generateExtraShadows;
	//CPointLights::aLights[id].m_pEntityToLight = entityAffected;
	//float intensity = 1.0;
	//if(visibleRadius * 0.75f <= dx.Magnitude())
	//{
	//	intensity = 1.0 - (dx.Magnitude() / visibleRadius - 0.75f) * 4.0;
	//}
	//CPointLights::aLights[id].m_fColorRed = red * intensity;
	//CPointLights::aLights[id].m_fColorGreen = green * intensity;
	//CPointLights::aLights[id].m_fColorBlue = blue * intensity;
	//CPointLights::NumLights++;


	LightData light;
	CVector lightpos;

	if(defaultType == PLTYPE_POINTLIGHT)
	{
		lightpos = pos;
	}
	else
	{
		lightpos.x = pos.x - dir.x;
		lightpos.y = pos.y - dir.y;
		lightpos.z = pos.z - dir.z;
	}

	float intensity = 10.0;
	light.pos = {lightpos.x, lightpos.y, lightpos.z};
	light.radius = radius;
	light.dir = dir;
	light.type = 0;
	light.color = {red * intensity, green * intensity, blue* intensity };

	//LightList.push_back(light);
	AddLight(light);
}

#define MultiplyMatrixWithVector(outPoint, m, point) ((CVector* ( __cdecl*)(CVector*, CMatrix*, CVector*))0x0059C890)(outPoint, m, point) 

void Lights::AddSpotLight(CVehicle* vehicle, int a, CMatrix* matrix, bool isRight)
{
	/*if(gRenderState == stageSphereMap)
		return;*/

	CVehicleModelInfo* pModelinfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
	CVector headlightPos = pModelinfo->m_pVehicleStruct->m_avDummyPos[a];

	if(a != 1 || headlightPos.x != 0.0f || headlightPos.y != 0.0f || headlightPos.z != 0.0f)
	{
		CVector pos;
		MultiplyMatrixWithVector(&pos, matrix, &headlightPos);
		if(!isRight)
		{
			float dist = headlightPos.x + headlightPos.x;
			pos = {pos.x - dist * matrix->right.x,
				   pos.y - dist * matrix->right.y,
				   pos.z - dist * matrix->right.z};
		}

		float distance = 0.05f;
		if(vehicle->m_nModelIndex == 530)
			distance = 0.5f;

		LightData light;

		light.pos = {pos.x + matrix->up.x * distance, pos.y + matrix->up.y * distance, pos.z + matrix->up.z * distance};
		light.radius = 10.0f;
		light.dir = {matrix->up.x, matrix->up.y, matrix->up.z};
		light.type = 1;
		light.color = {1.0f, 1.0f, 1.0f};
		AddLight(light);
	}
}