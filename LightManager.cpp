#include "LightManager.h"
#include "CommonD.h"
#include "CPointLights.h"
#include "CModelInfo.h"
#include "CCamera.h"

LightManager gLightManager;
#include "PointLightShadow.h"
bool CheckModelId(CEntity* entity)
{
	if (entity)
	{
		auto id = entity->m_nModelIndex;
		if (/*(id < 596 || id > 599) &&*/
			id == 438 ||
			id == 420 ||
			id == 416 ||
			id == 407 ||
			id == 544 ||
			id == 523)
			return false;
	}

	return true;
}
void CPointLights__AddLight(unsigned char, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity*e)
{

	auto castShadow = true;
	castShadow = castShadow && CheckModelId(e);
		//if(e)
		//PrintMessage("%d", e->m_nModelIndex);

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


	gLightManager.AddPointLight(point, direction, { red, green, blue }, radius, intensity, castShadow);
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
		float distance = 0.1f;
		if (vehicle->m_nModelIndex == 530)
			distance = 0.5f;

		SpotLight light;

		// Initialize the spot light

		light.SetRadius(30.0f);
		light.SetDirection({ matrix->up.x, matrix->up.y, matrix->up.z });
		light.SetColor({ 1.0f, 1.0f, 1.0f });
		light.SetIntensity(2.0);
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

 void CPointLights__AddLight1(unsigned char, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity* e)
 {
	 CEntity* ea;
	 _asm mov ea, esi;

	 //if (ea)
		// PrintMessage("%d", ea->m_nModelIndex);

	 CPointLights__AddLight(0, point, direction, radius, red, green, blue, 0, false, ea);
 }

void LightManager::Hook()
{   
	plugin::patch::RedirectCall(0x006AB80F, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x006ABBA6, CPointLights__AddLight1);

	plugin::Events::gameProcessEvent += []() 
	{
		gLightManager.ClearLights();

		auto coors = FindPlayerCoors(0);

		static float angle = 0.0f;

		angle += 2.0;
		
		float c = 360.0f / 4;

		float x, y,z,  r;

		r = 2.f;

		static RwRGBAReal randoColor[7];

		RwRGBAReal Peru = { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
		RwRGBAReal Pink = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
		RwRGBAReal Plum = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		RwRGBAReal PowderBlue = { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
		RwRGBAReal Purple = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
		 RwRGBAReal Red = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		 RwRGBAReal RosyBrown = { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
		//if (angle > 360.0)
		//{
			randoColor[0] = Peru;
			randoColor[1] = Pink;
			randoColor[2] = Plum;
			randoColor[3] = PowderBlue;
			randoColor[4] = Purple;
			randoColor[5] = Red;
			randoColor[6] = RosyBrown;
		//}
	
			static int key[]={0,4,3,2};
			int v = 0;
		if (angle > 360.0)
		{
			for (size_t i = 0; i < 4; i++)
				key[i] = plugin::Random(0, 6);
			 v = plugin::Random(0, 3);
		}

		if (angle > 360.0)
			angle = 0.0;
		//
		//for (size_t i = 0; i < 4; i++)
		//{
		//	float op1 = XMConvertToRadians(angle + c * i);

		//	float red = randoColor[key[i]].red;
		//	float green = randoColor[key[i]].green;
		//	float blue = randoColor[key[i]].blue;
		//	PrintMessage("%f %f %f", red, green, blue);
		//	x = coors.x + (i % 2 ? cos(op1) : sin(op1)) * r;
		//	y = coors.y + (i % 2 ? sin(op1) : cos(op1)) * r;
		//	auto s = (i % 1 ? sin(op1) : cos(op1));
		//	z = coors.z + s * r;
		//	gLightManager.AddPointLight({ x,y, z + 1.0f }, { 1, 1, 1 }, { red, green, blue }, 10.0f, 1.0);
		//}


		//gLightManager.AddPointLight({ coors.x ,coors.y+2, coors.z + 2.0f }, { 1, 1, 1 }, { 1, 1, 1 }, 10.0f, 1.0, true);

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

void LightManager::AddPointLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float intensity, bool castShadow)
{
	if (mPointLightCount > 59)
		return;

	//PointLight light;
	mPointLightList[mPointLightCount].SetPosition(position);
	mPointLightList[mPointLightCount].SetDirection(direction);
	mPointLightList[mPointLightCount].SetColor(color);
	mPointLightList[mPointLightCount].SetRadius(radius);
	mPointLightList[mPointLightCount].SetIntensity(intensity);
	mPointLightList[mPointLightCount].CastShadow(castShadow);
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
