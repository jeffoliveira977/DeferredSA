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
bool Lights::AddOmniLight(eLightMode defaultType, CVector pos, CVector dir, float radius, float red, float green, float blue, unsigned char fogType,
						   bool generateExtraShadows, void *entityAffected)
{

	if(gRenderState == stageSphereMap)
		return true;

	LightData light;
	CVector lightpos;

	if(defaultType == LIGHT_TYPE_OMNI)
	{
		lightpos = {pos.x, pos.y, pos.z};
	}
	else
	{
		lightpos.x = pos.x - dir.x;
		lightpos.y = pos.y - dir.y;
		lightpos.z = pos.z - dir.z;
	}

	light.pos = {lightpos.x, lightpos.y, lightpos.z};
	light.radius = radius;
	light.dir = {dir.x, dir.y, dir.z};
	light.type = 0.0;
	light.color = {red, green, blue};

	//LightList.push_back(light);
	AddLight(light);
	return true;
}

#define MultiplyMatrixWithVector(outPoint, m, point) ((CVector* ( __cdecl*)(CVector*, CMatrix*, CVector*))0x0059C890)(outPoint, m, point) 

void Lights::AddSpotLight(CVehicle* vehicle, int a, CMatrix* matrix, bool isRight)
{
	/*if(gRenderState == stageSphereMap)
		return;*/

	CVector pos;
	CVehicleModelInfo* pModelinfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
	CVector* headlightPos = &pModelinfo->m_pVehicleStruct->m_avDummyPos[a];

	MultiplyMatrixWithVector(&pos, matrix, headlightPos);
	//pos.FromMultiply3x3(*matrix, *headlightPos);
	if(!isRight)
	{
		float dist = headlightPos->x + headlightPos->x;
		pos = {pos.x - dist * matrix->right.x,
			   pos.y - dist * matrix->right.y,
			   pos.z - dist * matrix->right.z};
	}

	float distance = 0.02f;
	LightData light;

	light.pos = {pos.x + matrix->up.x * distance, pos.y + matrix->up.y * distance, pos.z + matrix->up.z * distance};
	 light.radius = 20.0;
	light.dir = {matrix->up.x, matrix->up.y, matrix->up.z};
	light.type = 1.0;
	light.color = {1.0, 1.0, 1.0};
	AddLight(light);
	LightList.push_back(light);
}