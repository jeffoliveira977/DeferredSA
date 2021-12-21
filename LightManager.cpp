#include "LightManager.h"
#include "CommonD.h"
#include "CPointLights.h"
#include "CModelInfo.h"
#include "CCamera.h"

LightManager gLightManager;

void CPointLights__AddLight(unsigned char, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity*)
{
	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 100.0;
	CVector dx = CVector(point.x, point.y, point.z) - camPos;

	if (gLightManager.GetPointLightCount() > 29 || dx.Magnitude() >= visibleRadius)
		return;

	float intensity = 3.0;

	gLightManager.AddPointLight(point, direction, { red, green, blue }, radius, intensity);
}

void AddVehicleSpotLight(CVehicle* vehicle)
{
	if (gLightManager.GetSpotLightCount() > 28)
			return;

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

		// Initialize the spot light

		light.SetRadius(30.0f);
		light.SetDirection({ matrix->up.x, matrix->up.y, matrix->up.z });
		light.SetColor({ 1.0f, 1.0f, 1.0f });
		light.SetIntensity(3.0);
		light.SetAngle(30.0);

		CVector position;
		MultiplyMatrixWithVector(&position, matrix, &headlightPos);

		if (bHEAD_L)
		{
			light.SetPosition({ position.x + matrix->up.x * distance,
							   position.y + matrix->up.y * distance,
							   position.z + matrix->up.z * distance });
			light.Update();
			gLightManager.AddSpotLight(light);
		};

		if (bHEAD_R)
		{
			float of = headlightPos.x + headlightPos.x;
			position = { position.x - of * matrix->right.x,
						position.y - of * matrix->right.y,
						position.z - of * matrix->right.z };

			light.SetPosition({ position.x + matrix->up.x * distance,
							   position.y + matrix->up.y * distance,
							   position.z + matrix->up.z * distance });
			light.Update();
			gLightManager.AddSpotLight(light);
		}
	}
}

void __stdcall CVehicle__DoHeadLightReflection(CVehicle*, int, CMatrix*, bool)
{}

void __fastcall CVehicle__DoHeadLightBeam(CVehicle*, CMatrix*, unsigned int, unsigned char, unsigned char)
{}

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

	plugin::patch::Nop(0x6E27E6, 5);
	plugin::patch::RedirectJump(0x7000E0, CPointLights__AddLight);
	plugin::patch::RedirectJump(0x6E0E20, CVehicle__DoHeadLightBeam);
	plugin::patch::RedirectJump(0x6E1720, CVehicle__DoHeadLightReflection);
}

float rwV3D_Dist( XMFLOAT3 a,  XMFLOAT3 b)
{
	float
		dX = b.x - a.x,
		dY = b.y - a.y,
		dZ = b.z - a.z;

	return sqrt(dX * dX + dY * dY + dZ * dZ);
}

#include "GTADef.h"
#include "CScene.h"
void LightManager::SortByDistance(XMFLOAT3 from)
{
	auto campos = RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera)));

	for (int index = 0; index < mPointLightList.size(); index++)
	{
		float sqrDistanceToCam = XMVectorGetX(XMVectorMax(g_XMOne, XMVector3Length(XMLoadFloat3((XMFLOAT3*)campos))- XMLoadFloat3(&mPointLightList[index].GetPosition()) ));

		//compute a value to determine light order 
		mPointLightList[index].mPriority = 1000.0f * mPointLightList[index].GetRadius() / std::max(1.0f, sqrDistanceToCam);
	}

	std::sort(mPointLightList.begin(), mPointLightList.end(), [&](PointLight& a, PointLight& b)
		{
			return (int)(b.mPriority - a.mPriority);
		});

	//std::sort(mPointLightList.begin(), mPointLightList.end(), [&](PointLight& a, PointLight& b)
	//	{
	//		float
	//			dX = from.x - a.GetPosition().x,
	//			dY = from.y - a.GetPosition().y,
	//			dZ = from.z - a.GetPosition().z;
	//		float sq = sqrt(dX * dX + dY * dY + dZ * dZ);

	//		bool is = sq >= 100.0;


	//		dX = from.x - b.GetPosition().x,
	//			dY = from.y - b.GetPosition().y,
	//			dZ = from.z - b.GetPosition().z;
	//		sq = sqrt(dX * dX + dY * dY + dZ * dZ);
	//		bool is2 = sq >= 100.0;

	//		return is && is2;
	//	});
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
	light.Update();
	mPointLightList.push_back(light);
}

void LightManager::ClearLights()
{
	mPointLightList.clear();
	mSpotLightList.clear();
}

size_t LightManager::GetSpotLightCount()
{
	return mSpotLightList.size();
}

size_t LightManager::GetPointLightCount()
{
	return mPointLightList.size();
}

SpotLight LightManager::GetSpotLightAt(int i)
{
	return mSpotLightList[i];
}

PointLight LightManager::GetPointLightAt(int i)
{
	return mPointLightList[i];
}
