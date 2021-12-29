#include "LightManager.h"
#include "CommonD.h"
#include "CPointLights.h"
#include "CModelInfo.h"
#include "CCamera.h"

LightManager gLightManager;
#include "PointLightShadow.h"

void CPointLights__AddLight(unsigned char, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity*e)
{

	//if (e)
	//	if (e->m_nModelIndex == 1215)
	//		//PrintMessage("%f %f %f %d", point.x, point.y, point.z, e->m_nModelIndex);
	//		return;
	//direction = point;
	//if (direction.z <= 30.0)
	//	direction.z += 4.0;
	
	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 80.0;
	CVector dx = CVector(point.x, point.y, point.z) - camPos;
	float intensity = 3.0;

	if (dx.Magnitude() >= visibleRadius)
		return;


	gLightManager.AddPointLight(point, direction, { red, green, blue }, radius, intensity);
}

void AddVehicleSpotLight(CVehicle* vehicle)
{
	if (gLightManager.GetSpotLightCount() > 28)
			return;

	CVehicleModelInfo* pModelinfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
	CVector headlightPos = pModelinfo->m_pVehicleStruct->m_avDummyPos[0];

	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 100.0;
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
		light.SetIntensity(10.0);
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

 int LightManager::mPointLightCount =0;
 /*std::vector<*/PointLight/*>*/ LightManager::mPointLightList[60];

void LightManager::Hook()
{
	plugin::Events::gameProcessEvent += []() 
	{
		gLightManager.ClearLights();

		auto coors = FindPlayerCoors(0);
		//PrintMessage("%f %f %f", coors.x, coors.y, coors.z);
		gLightManager.AddPointLight({ coors.x,coors.y,coors.z+2.0f }, {0.32, 0.8, 0.5}, { 1, 1, 1 }, 10.0f, 1);

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

float rwV3D_Dist(XMFLOAT3 a, XMFLOAT3 b)
{
	float
		dX = b.x - a.x,
		dY = b.y - a.y,
		dZ = b.z - a.z;

	return sqrt(dX * dX + dY * dY + dZ * dZ);
}

#include "GTADef.h"
#include "CScene.h"

void LightManager::SortLights()
{
	auto position = (XMFLOAT3*)RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera)));

	//for (int index = 0; index < mPointLightCount; index++)
	//{
	//	float sqrDistanceToCam = XMVectorGetX(XMVectorMax(g_XMOne, XMVector3Length(XMLoadFloat3((XMFLOAT3*)position)) - XMLoadFloat3(&mPointLightList[index].GetPosition())));

	//	//compute a value to determine light order 
	//	mPointLightList[index].mPriority = 1000.0f * mPointLightList[index]. GetRadius() / std::max(1.0f, sqrDistanceToCam);
	//}

	//std::sort(&mPointLightList[0], &mPointLightList[59], [&](PointLight a, PointLight b)
	//{
	//		return (b.mPriority < a.mPriority);
	//});

	sort(&mPointLightList[0], &mPointLightList[mPointLightCount-1], [&]( PointLight& a,  PointLight& b)
		{
			return (rwV3D_Dist(a.GetPosition(), *position) < rwV3D_Dist(b.GetPosition(), *position));
		});
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
	if (mPointLightCount > 59)
		return;

	mPointLightList[mPointLightCount++] = pointlight;
	//mPointLightList.push_back(pointlight);
}

void LightManager::AddPointLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float intensity)
{
	if (mPointLightCount > 59)
		return;

	//PointLight light;
	mPointLightList[mPointLightCount].SetPosition(position);
	mPointLightList[mPointLightCount].SetDirection(direction);
	mPointLightList[mPointLightCount].SetColor(color);
	mPointLightList[mPointLightCount].SetRadius(radius);
	mPointLightList[mPointLightCount].SetIntensity(intensity);
	mPointLightList[mPointLightCount].Update();
	//mPointLightList[mPointLightCount++] = light;

	mPointLightCount++;

	// mPointLightList.push_back(light);
}

void LightManager::ClearLights()
{
	mPointLightCount = 0;
	// mPointLightList.clear();
	mSpotLightList.clear();

	for (size_t i = 0; i < mPointLightCount; i++)
	{
		mPointLightList[i].mCastShadow = false;
	}
}

size_t LightManager::GetSpotLightCount()
{
	return mSpotLightList.size();
}

size_t LightManager::GetPointLightCount()
{
	return mPointLightCount;
	//return mPointLightList.size();
}

SpotLight LightManager::GetSpotLightAt(int i)
{
	return mSpotLightList[i];
}

PointLight* LightManager::GetPointLightAt(int i)
{
	return &mPointLightList[i];
}
