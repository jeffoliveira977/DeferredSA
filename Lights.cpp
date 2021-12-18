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
	patch::Nop(0x6E2718, 2);
	patch::Nop(0x6E27E6, 5);

	patch::RedirectJump(0x7000E0, AddOmniLight);
	patch::RedirectJump(0x6E0E20, AddSpotLight);
	patch::RedirectJump(0x006E1720, DoHeadLightReflection);

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
	float visibleRadius = 250.0;
	CVector dx = pos - camPos;

	if (dx.Magnitude() >= visibleRadius)
	{
		return;
	}

	if(CPointLights::NumLights >= 46)
	{
		return;
	}

	float intensity = 3.0;

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

	//float intensity = 10.0;
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

}

void __thiscall Lights::DoHeadLightReflection(CVehicle* vehicle, CMatrix* matrix, unsigned int flags, unsigned char left, unsigned char right)
{
	CVehicleModelInfo* pModelinfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
	CVector headlightPos = pModelinfo->m_pVehicleStruct->m_avDummyPos[0];

	if (headlightPos.x != 0.0f || headlightPos.y != 0.0f || headlightPos.z != 0.0f)
	{
		float distance = 0.05f;
		if (vehicle->m_nModelIndex == 530)
			distance = 0.5f;

		LightData light;

		light.radius = 30.0f;
		light.dir = { matrix->up.x, matrix->up.y, matrix->up.z };
		light.type = 1;
		float intensity = 1.0;
		light.color = { 1.0f * intensity, 1.0f * intensity, 1.0f * intensity };
		
		CVector pos;
		MultiplyMatrixWithVector(&pos, matrix, &headlightPos);

		if (right)
		{
			light.pos = { pos.x + matrix->up.x * distance, pos.y + matrix->up.y * distance, pos.z + matrix->up.z * distance };
			AddLight(light);
		}

		if (left)
		{
			float of = headlightPos.x + headlightPos.x;
			pos = { pos.x - of * matrix->right.x,
				  pos.y - of * matrix->right.y,
				  pos.z - of * matrix->right.z};

			light.pos = { pos.x + matrix->up.x * distance, pos.y + matrix->up.y * distance, pos.z + matrix->up.z * distance };
			AddLight(light);
		}
	}
}