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
		if (id == 596 ||
			id == 597 ||
			id == 598 ||
			id == 599 ||
			id == 438 ||
			id == 420 ||
			id == 416 ||
			id == 407 ||
			id == 544 ||
			id == 523)
			return true;
	}

	return false;
}
CEntity* CurrEntity = nullptr;

void CPointLights__AddLight(unsigned char defaultType, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity* e)
{
	/*if (e)
		PrintMessage("%d", e->m_nModelIndex);*/

	auto castShadow = true;
	castShadow = CheckModelId(CurrEntity);

	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 100.0f;
	CVector dx = CVector(point.x, point.y, point.z) - camPos;

	if (dx.Magnitude() >= visibleRadius)
		return;

	float intensity = 1.0;
	gLightManager.AddPointLight(point, direction, { red, green, blue }, radius, intensity, castShadow);
}

void AddVehicleSpotLight(CVehicle* vehicle)
{	

	CVehicleModelInfo* pModelinfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
	CVector headlightPos = pModelinfo->m_pVehicleStruct->m_avDummyPos[0];
	CVector tailLightPos = pModelinfo->m_pVehicleStruct->m_avDummyPos[1];

	auto automobile = reinterpret_cast<CAutomobile*>(vehicle);

	auto bHEAD_R = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_FRONT_RIGHT);
	auto bHEAD_L = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_FRONT_LEFT);
	auto bREAR_R = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_REAR_RIGHT);
	auto bREAR_L = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_REAR_LEFT);

	auto matrix = vehicle->GetMatrix();
	//if (headlightPos.x != 0.0f || headlightPos.y != 0.0f || headlightPos.z != 0.0f)
	//{
		float distance = 0.1f;
		if (vehicle->m_nModelIndex == 530)
			distance = 0.5f;

		SpotLight light;

		// Initialize the spot light
		light.SetIntensity(1.0);

		CVector position;
		if (bHEAD_L || bHEAD_R)
		{
			light.SetRadius(30.0f);
			light.SetDirection({ matrix->up.x, matrix->up.y, matrix->up.z });
			light.SetColor({ 1.0f, 1.0f, 1.0f });
			light.SetAngle(30.0);

			MultiplyMatrixWithVector(&position, matrix, &headlightPos);

			if (bHEAD_L)
			{
				light.SetPosition({position.x, position.y, position.z });
				light.Update();
				gLightManager.AddSpotLight(light);
			};

			if (bHEAD_R)
			{
				position = position - (headlightPos.x + headlightPos.x) * matrix->right;
				light.SetPosition({position.x, position.y, position.z});
				light.Update();
				gLightManager.AddSpotLight(light);
			}
		}
		
		if (bREAR_R || bREAR_L)
		{
			light.SetDirection({ -vehicle->m_matrix->up.x, -vehicle->m_matrix->up.y,-vehicle->m_matrix->up.z });

			if (vehicle->m_fBreakPedal > 0.0f)
				light.SetColor({ 1.0f, 0.0f, 0.0f });
			else
				light.SetColor({ 0.8f, 0.0f, 0.0f });
			
			if (vehicle->m_nVehicleSubClass == eVehicleType::VEHICLE_BIKE || vehicle->m_nVehicleClass == eVehicleType::VEHICLE_BIKE)
			{
				light.SetAngle(30.0);
				light.SetRadius(vehicle->m_fBreakPedal > 0.0f ? 7.0 : 3.0f);
				light.SetIntensity(3.0);
			}
			else
			{
				light.SetAngle(100.0);
				light.SetRadius(vehicle->m_fBreakPedal > 0.0f ? 7.0 : 3.0f);
			}

			MultiplyMatrixWithVector(&position, matrix, &tailLightPos);

			if (bREAR_L)
			{
				light.SetPosition({position.x, position.y, position.z});
				light.Update();
				gLightManager.AddSpotLight(light);
			};

			if (bREAR_R)
			{
				position = position - (tailLightPos.x + tailLightPos.x) * matrix->right;
				light.SetPosition({position.x, position.y, position.z});
				light.Update();
				gLightManager.AddSpotLight(light);
			}
		//}
	}
}

void __stdcall CVehicle__DoHeadLightReflection(CVehicle*, int, CMatrix*, bool)
{}

void __fastcall CVehicle__DoHeadLightBeam(CVehicle*, CMatrix*, unsigned int, unsigned char, unsigned char)
{}

LightManager::LightManager()
{}

int LightManager::mSpotLightCount = 0;
int LightManager::mPointLightCount = 0;
PointLight LightManager::mPointLightList[160];
SpotLight LightManager::mSpotLightList[160];
unordered_map<CEntity*, bool>  LightManager::mDontCull;
 void CPointLights__AddLight1(unsigned char defaultType, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity*)
 {
	 _asm mov CurrEntity, esi;

	 if (CurrEntity == nullptr)
		 return;

	 // increase siren radius
	 radius = 50.0f;
	
	 if (CurrEntity->m_nModelIndex == MODEL_CABBIE ||
		 CurrEntity->m_nModelIndex == MODEL_TAXI)
		 radius = 5.0f;

	 if (!CheckModelId(CurrEntity))
	 {
		 radius = 10.0f;
		 gLightManager.mDontCull[CurrEntity] = true;
	 }

	 CPointLights__AddLight(defaultType, point, direction, radius, red, green, blue, 0, false, CurrEntity);
 }


void __declspec(naked) CPointLights__AddLight_HOOK()
{
	 __asm
	 {
		mov CurrEntity, esi
		call CPointLights__AddLight
		mov edx, 6AB814h
		jmp edx
	 }
}

void __declspec(naked) CPointLights__AddLight_HOOK1()
{
	__asm
	{
		mov CurrEntity, esi
		call CPointLights__AddLight
		mov edx, 6ABBABh
		jmp edx
	}
}
#include "CRenderer.h"

#include "CPtrListDoubleLink.h"
#include "CWorld.h"
#include "CPools.h"
void LightManager::Hook()
{   
	//plugin::patch::RedirectJump(0x6AB80F, CPointLights__AddLight_HOOK1);
	//plugin::patch::RedirectJump(0x6ABBA6, CPointLights__AddLight_HOOK1);

	plugin::patch::RedirectCall(0x6AB80F, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x6ABBA6, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x6BD641, CPointLights__AddLight1);

	plugin::patch::RedirectCall(0x6FD105, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x6FD347, CPointLights__AddLight1);


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
			{
				if (!vehicle->m_nVehicleFlags.bLightsOn)
					continue;

				CVector camPos = TheCamera.GetPosition();
				float visibleRadius = 100.0;
				CVector dx = vehicle->GetPosition() - camPos;

				if (dx.Magnitude() >= visibleRadius)
					continue;

				AddVehicleSpotLight(vehicle);
			}
		} 
	};


	plugin::patch::Nop(0x6E28E7, 5);
	plugin::patch::Nop(0x6E27E6, 5);
	plugin::patch::RedirectJump(0x7000E0, CPointLights__AddLight);
	plugin::patch::RedirectJump(0x6E0E20, CVehicle__DoHeadLightBeam);
	plugin::patch::RedirectJump(0x6E1720, CVehicle__DoHeadLightReflection);
}


#include "GTADef.h"
#include "CScene.h"

void LightManager::SortSpotLights()
{
	auto cameraPos = XMLoadFloat3((XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos);
	auto coors = FindPlayerCoors(-1);
	//auto cameraPos = XMLoadFloat3((XMFLOAT3*)&coors);

	sort(&mSpotLightList[0], &mSpotLightList[mSpotLightCount - 1], [&](SpotLight& a, SpotLight& b)
		{
			auto lenA = XMVector3Length(cameraPos - XMLoadFloat3(&a.GetPosition()));
			auto lenB = XMVector3Length(cameraPos - XMLoadFloat3(&b.GetPosition()));

			return XMVector3Less(lenA, lenB);

		});
}

float rwV3D_Dist2( XMFLOAT3& a,  XMFLOAT3& b)
{
	float
		dX = b.x - a.x,
		dY = b.y - a.y,
		dZ = b.z - a.z;

	return sqrt(dX * dX + dY * dY + dZ * dZ);
}
void LightManager::SortPointLights()
{
	auto pos = (XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos;
	 auto cameraPos = XMLoadFloat3((XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos);
	auto coors = FindPlayerCoors(-1);
	//auto cameraPos = XMLoadFloat3((XMFLOAT3*)&coors);

	/*for (int index = 0; index < mPointLightCount; index++)
	{
		float sqrDistanceToCam = XMVectorGetX(XMVectorMax(g_XMOne, XMVector3Length(cameraPos - XMLoadFloat3(&mPointLightList[index].GetPosition()))));

		mPointLightList[index].mPriority = index;
	}
	*/
	//sort(&mPointLightList[0], &mPointLightList[mPointLightCount-1], [&](PointLight& a, PointLight& b)
	//	{
	//		auto lenA = XMVector3Length(cameraPos - XMLoadFloat3(&a.GetPosition()));
	//		auto lenB = XMVector3Length(cameraPos - XMLoadFloat3(&b.GetPosition()));

	//		return XMVector3Less(lenA, lenB);

	//	});

	std::sort(&mPointLightList[0], &mPointLightList[159], [&]( PointLight& a,  PointLight& b)
		{
			return (rwV3D_Dist2(a.GetPosition(), *pos) < rwV3D_Dist2(b.GetPosition(), *pos));
		});

	/*std::ofstream myfile;
	myfile.open("example2.txt");

	for (size_t i = 0; i < mPointLightCount; i++)
	{
		myfile << mPointLightList[i].mPriority;
		myfile << "\n";
	}

	myfile.close();*/
}

void LightManager::AddSpotLight(SpotLight spotlight)
{
	if (mSpotLightCount > 59)
		return;

	mSpotLightList[mSpotLightCount++] = spotlight;
}

void LightManager::AddSpotLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float angle, float intensity)
{
	SpotLight light;

	mSpotLightList[mSpotLightCount].SetPosition(position);
	mSpotLightList[mSpotLightCount].SetDirection(direction);
	mSpotLightList[mSpotLightCount].SetColor(color);
	mSpotLightList[mSpotLightCount].SetRadius(radius);
	mSpotLightList[mSpotLightCount].SetIntensity(intensity);
	mSpotLightList[mSpotLightCount].SetAngle(angle);
	mSpotLightList[mSpotLightCount].Update();

	mSpotLightCount++;
}

void LightManager::AddPointLight(PointLight pointlight)
{
	if (mPointLightCount >= 159)
		return;

	mPointLightList[mPointLightCount++] = pointlight; gLightManager.SortPointLights();
}

void LightManager::AddPointLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float intensity, bool castShadow)
{
	if (mPointLightCount >= 159)
		return;

	mPointLightList[mPointLightCount].SetPosition(position);
	mPointLightList[mPointLightCount].SetDirection(direction);
	mPointLightList[mPointLightCount].SetColor(color);
	mPointLightList[mPointLightCount].SetRadius(radius);
	mPointLightList[mPointLightCount].SetIntensity(intensity);
	mPointLightList[mPointLightCount].CastShadow(castShadow);
	mPointLightList[mPointLightCount].Update();

	mPointLightCount++;
	gLightManager.SortPointLights();
}

void LightManager::ClearLights()
{
	mPointLightCount = 0;
	mSpotLightCount = 0;

	ZeroMemory(mPointLightList, sizeof(mPointLightList));
	for (size_t i = 0; i < mPointLightCount; i++)
	{
		mPointLightList[i].mDrawShadow = true;
		mPointLightList[i].mCastShadow = false;
	}
	mDontCull.clear();
}

size_t LightManager::GetSpotLightCount()
{
	return mSpotLightCount;
}

size_t LightManager::GetPointLightCount()
{
	return mPointLightCount;
}

SpotLight *LightManager::GetSpotLightAt(int i)
{
	return &mSpotLightList[i];
}

PointLight* LightManager::GetPointLightAt(int i)
{
	return &mPointLightList[i];
}
