#include "LightManager.h"
#include "CommonD.h"
#include "CPointLights.h"
#include "CModelInfo.h"
#include "CCamera.h"

LightManager gLightManager;

void CPointLights__AddLight(unsigned char, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity*)
{
	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 250.0;
	CVector dx = CVector(point.x, point.y, point.z) - camPos;

	if (dx.Magnitude() >= visibleRadius)
		return;
	
	float intensity = 3.0;

	gLightManager.AddPointLight(point, direction, {red, green, blue}, radius, intensity);
}

void AddVehicleSpotLight(CVehicle* vehicle)
{
	CVehicleModelInfo* pModelinfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
	CVector headlightPos = pModelinfo->m_pVehicleStruct->m_avDummyPos[0];

	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 250.0;
	CVector dx = vehicle->GetPosition() - camPos;

	if (dx.Magnitude() >= visibleRadius)	
		return;

	bool lightOn = vehicle->GetVehicleLightsStatus();
	auto lights = vehicle->m_renderLights;
	//flagLightsOn = (vehicle->m_nFlags[0] >> 6) & 1;
	if (!lightOn)
		return;

	auto automobile = reinterpret_cast<CAutomobile*>(vehicle);

	auto bHEAD_R = (automobile && !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_FRONT_RIGHT));
	auto bHEAD_L = (automobile && !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_FRONT_LEFT));

	auto matrix = vehicle->GetMatrix();
	if (headlightPos.x != 0.0f || headlightPos.y != 0.0f || headlightPos.z != 0.0f)
	{
		float distance = 0.05f;
		if (vehicle->m_nModelIndex == 530)
			distance = 0.5f;

		SpotLight light;
		light.SetRadius(30.0f);
		light.SetDirection({matrix->up.x, matrix->up.y, matrix->up.z});
		
		light.SetColor({1.0f, 1.0f, 1.0f});
		light.SetIntensity(1.0);

		CVector position;
		MultiplyMatrixWithVector(&position, matrix, &headlightPos);

		if (bHEAD_L)
		{
			light.SetPosition({position.x + matrix->up.x * distance,
							   position.y + matrix->up.y * distance,
							   position.z + matrix->up.z * distance});
			
			gLightManager.AddSpotLight(light);
		};

		if (bHEAD_R)
		{
			float of = headlightPos.x + headlightPos.x;
			position = {position.x - of * matrix->right.x,
						position.y - of * matrix->right.y,
						position.z - of * matrix->right.z };

			light.SetPosition({position.x + matrix->up.x * distance,
							   position.y + matrix->up.y * distance,
							   position.z + matrix->up.z * distance});

			gLightManager.AddSpotLight(light);
		}
	}
}

LightManager::LightManager()
{}

void LightManager::Hook()
{
	plugin::Events::gameProcessEvent += []() 
	{
		gLightManager.ClearLights();

		auto pool = CPools::ms_pVehiclePool;
		for (int i = 0; i < pool->m_nSize; ++i)
		{
			auto* vehicle = pool->GetAt(i);
			if (vehicle)
				AddVehicleSpotLight(vehicle);
		}
	};

	plugin::patch::Nop(0x6E2718, 2);
	plugin::patch::Nop(0x6E27E6, 5);
	plugin::patch::RedirectJump(0x7000E0, CPointLights__AddLight);
}

void LightManager::AddSpotLight(SpotLight spotlight)
{
	mSpotLightList.push_back(spotlight);
}

void LightManager::AddSpotLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float angle, float intensity)
{
	SpotLight light;

	light.SetPosition(position);
	light.SetDirection(direction);
	light.SetColor(color);
	light.SetRadius(radius);
	light.SetIntensity(intensity);
	light.SetAngle(angle);

	mSpotLightList.push_back(light);
}

void LightManager::AddPointLight(PointLight pointlight)
{
	mPointLightList.push_back(pointlight);
}

void LightManager::AddPointLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float intensity)
{
	PointLight light;

	light.SetPosition(position);
	light.SetDirection(direction);
	light.SetColor(color);
	light.SetRadius(radius);
	light.SetIntensity(intensity);

	mPointLightList.push_back(light);
}

void LightManager::ClearLights()
{
	mPointLightList.clear();
	mSpotLightList.clear();
}

void LightManager::GetSpotLightCount()
{
	mSpotLightList.size();
}

void LightManager::GetPointLightCount()
{
	mPointLightList.size();
}

SpotLight LightManager::GetSpotLightAt(int i)
{
	return mSpotLightList[i];
}

PointLight LightManager::GetPointLightAt(int i)
{
	return mPointLightList[i];
}
