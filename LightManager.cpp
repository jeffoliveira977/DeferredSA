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
template<typename T>
T saturate(T val) {
	return std::min(std::max(val, 0.0f), 1.0f);
}
void CPointLights__AddLight(unsigned char defaultType, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity* e)
{
	auto castShadow = true;
	if (CheckModelId(CurrEntity))
		castShadow = false;

	CVector camPos = TheCamera.GetPosition();
	float visibleRadius = 100.0f;
	CVector dx = CVector(point.x, point.y, point.z) - camPos;

	if (dx.Magnitude() >= visibleRadius)
		return;

	visibleRadius = 100.0;
	float attenuation = 1 - powf(saturate((dx.Magnitude() / visibleRadius)), 2.0f);
	attenuation *= attenuation;
	gLightManager.AddPointLight(point, direction, { red, green, blue }, radius, attenuation, castShadow);
}

void AddVehicleSpotLight(CVehicle* vehicle)
{
	if (!vehicle->m_nVehicleFlags.bLightsOn)
		return;

	float visibleRadius = 100.0;
	CVector dx = vehicle->GetPosition() - TheCamera.GetPosition();

	if (dx.Magnitude() >= visibleRadius)
		return;

	auto modelInfo = reinterpret_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));

	auto automobile = reinterpret_cast<CAutomobile*>(vehicle);
	auto frontRight = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_FRONT_RIGHT);
	auto frontLeft = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_FRONT_LEFT);
	auto rearRight = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_REAR_RIGHT);
	auto rearLeft = !automobile->m_damageManager.GetLightStatus(eLights::LIGHT_REAR_LEFT);

	auto vehicleMatrix = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(vehicle->GetMatrix()));

	float distance = 0.6f;
	if (vehicle->m_nModelIndex == 530)
		distance = 0.5f;

	XMFLOAT3 direction;
	XMFLOAT3 position;

	SpotLight light;
	float attenuation = 1 - powf(saturate((dx.Magnitude() / visibleRadius)), 2.0f);
	attenuation *= attenuation;
	light.SetIntensity(attenuation);
	light.mCastShadow = true;
	light.mUsePattern = true;

	auto vehicleStruct = modelInfo->m_pVehicleStruct;
	if (frontLeft || frontRight)
	{
		if (FindPlayerVehicle(-1, true) == vehicle)
		{
			light.SetAngle(45.0);
			light.SetRadius(60.0f);
		}
		else
		{
			light.SetAngle(40.0);
			light.SetRadius(20.0f);
			//light.mCastShadow = false;
		}

		XMStoreFloat3(&direction, vehicleMatrix.r[1]);
		light.SetDirection(direction);

		light.SetColor({ 1.0f, 1.0f , 1.0f });

		auto headlightPos = XMLoadFloat3((XMFLOAT3*)&vehicleStruct->m_avDummyPos[0]);
		auto headlightTranform = XMVector3Transform(headlightPos, vehicleMatrix) + vehicleMatrix.r[1] * distance;

		XMStoreFloat3(&position, headlightTranform);

		if (frontRight)
		{
			light.SetPosition({ position.x, position.y, position.z });
			light.Update();
			gLightManager.AddSpotLight(light);
		};

		if (frontLeft)
		{
			headlightTranform -= XMVectorSplatX(headlightPos) * 2.0f * vehicleMatrix.r[0];
			XMStoreFloat3(&position, headlightTranform);
			light.SetPosition({position.x, position.y, position.z});
			light.Update();
			gLightManager.AddSpotLight(light);
		}
	}

	visibleRadius = 50.0;

	if (dx.Magnitude() >= visibleRadius)
		return;

	if (rearRight || rearLeft)
	{
		light.mCastShadow = false;
		light.mUsePattern = false;
		XMStoreFloat3(&direction, -vehicleMatrix.r[1]);
		light.SetDirection(direction);

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

		auto tailLightPos = XMLoadFloat3((XMFLOAT3*)&vehicleStruct->m_avDummyPos[1]);
		auto tailLighTranform = XMVector3Transform(tailLightPos, vehicleMatrix);

		XMStoreFloat3(&position, tailLighTranform);

		if (rearRight)
		{
			light.SetPosition({ position.x, position.y, position.z });
			light.Update();
			gLightManager.AddSpotLight(light);
		};

		if (rearLeft)
		{
			tailLighTranform -= XMVectorSplatX(tailLightPos) * 2.0f * vehicleMatrix.r[0];
			XMStoreFloat3(&position, tailLighTranform);
			light.SetPosition({ position.x, position.y, position.z });
			light.Update();
			gLightManager.AddSpotLight(light);
		}
	}
}

std::unordered_map<CVehicle*, bool> gVehicleInScreen;
class veh
{
public:
	static void  __thiscall CVehicle__DoHeadLightReflection(CVehicle* vehicle, CMatrix* matrix, unsigned int, unsigned char, unsigned char)
	{
	}
	static void  __thiscall CVehicle__DoHeadLightBeam(CVehicle* vehicle, int, CMatrix*, unsigned char isRight)
	{}

};

DWORD RETURN_CVehicle_DoVehicleLights = 0x6e1a68;
CVehicle* pLightsVehicleInterface;

void _declspec(naked) HOOK_CVehicle_DoVehicleLights()
{
	_asm
	{
		push ecx
		call AddVehicleSpotLight;
		pop ecx			
		mov al, byte ptr ds : [00C1CC18h]
		sub esp, 3Ch
		jmp RETURN_CVehicle_DoVehicleLights
	}
}

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
#include "CExplosion.h"
void CPointLights__AddLight2(unsigned char defaultType, XMFLOAT3 point, XMFLOAT3 direction, float radius, float red, float green, float blue, unsigned char, bool, CEntity*)
{
	PrintMessage("%f %f %f", point.x, point.y, point.z);

	float intensity = 1.0f;

	gLightManager.AddPointLight(point, direction, { red, green, blue }, radius, intensity, true);
}

#include "CRenderer.h"

#include "CPtrListDoubleLink.h"
#include "CWorld.h"
#include "CPools.h"
void LightManager::Hook()
{   
	//plugin::patch::RedirectJump(0x6AB80F, CPointLights__AddLight_HOOK1);
	//plugin::patch::RedirectJump(0x6ABBA6, CPointLights__AddLight_HOOK1);

	plugin::patch::Nop(0x73785D, 2);

	plugin::patch::RedirectCall(0x6AB80F, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x6ABBA6, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x6BD641, CPointLights__AddLight1);

	plugin::patch::RedirectCall(0x6FD105, CPointLights__AddLight1);
	plugin::patch::RedirectCall(0x6FD347, CPointLights__AddLight1);

	plugin::patch::RedirectCall(0x737849, CPointLights__AddLight2);
	plugin::patch::RedirectCall(0x7378C1, CPointLights__AddLight2);

	plugin::Events::gameProcessEvent.before += []() 
	{
		gLightManager.ClearLights();
		gVehicleInScreen.clear();
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

	/*	auto pool = CPools::ms_pVehiclePool;
		for (int i = 0; i < pool->m_nSize; ++i)
		{
			auto* vehicle = pool->GetAt(i);
			if (vehicle && gVehicleInScreen[vehicle])
			{

				if (!vehicle->m_nVehicleFlags.bLightsOn)
					continue;

				CVector camPos = TheCamera.GetPosition();
				float visibleRadius = 200.0;
				CVector dx = vehicle->GetPosition() - camPos;

				if (dx.Magnitude() >= visibleRadius)
					continue;

				AddVehicleSpotLight(vehicle);
			}
		} */

	};


	plugin::patch::Nop(0x6E28E7, 5);
	plugin::patch::Nop(0x6E27E6, 5);
	plugin::patch::RedirectJump(0x7000E0, CPointLights__AddLight);
	plugin::patch::RedirectJump(0x6E0E20, veh::CVehicle__DoHeadLightBeam);
	plugin::patch::RedirectJump(0x6E1720, veh::CVehicle__DoHeadLightReflection);
	//plugin::patch::RedirectJump(0x6E1780, veh::DoTailLightEffect);
	//plugin::patch::RedirectJump(0x6E1A60, veh::DoVehicleLights);



	//plugin::patch::RedirectCall(0x6ABCB9, veh::DoVehicleLights);
	//plugin::patch::RedirectCall(0x6E1A60, veh::DoVehicleLights);
	//plugin::patch::RedirectCall(0x6F5570, veh::DoVehicleLights);
	//plugin::patch::RedirectCall(0x6F55A1, veh::DoVehicleLights);
	//plugin::patch::RedirectCall(0x6F55B4, veh::DoVehicleLights);

	plugin::patch::RedirectJump(0x6e1a60, HOOK_CVehicle_DoVehicleLights);
}


#include "GTADef.h"
#include "CScene.h"
float rwV3D_Dist2(XMFLOAT3& a, XMFLOAT3& b)
{
	float
		dX = b.x - a.x,
		dY = b.y - a.y,
		dZ = b.z - a.z;

	return sqrt(dX * dX + dY * dY + dZ * dZ);
}
void LightManager::SortSpotLights()
{
	auto cameraPos = XMLoadFloat3((XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos);
	auto coors = FindPlayerCoors(-1);
	//auto cameraPos = XMLoadFloat3((XMFLOAT3*)&coors);

	//sort(&mSpotLightList[0], &mSpotLightList[159], [&](SpotLight& a, SpotLight& b)
	//	{
	//		auto lenA = XMVector3Length(cameraPos - XMLoadFloat3(&a.GetPosition()));
	//		auto lenB = XMVector3Length(cameraPos - XMLoadFloat3(&b.GetPosition()));

	//		return XMVector3Less(lenA, lenB);

	//	});
	auto pos = (XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos;
	std::sort(&mSpotLightList[0], &mSpotLightList[159], [&](SpotLight& a, SpotLight& b)
		{
			return (rwV3D_Dist2(a.GetPosition(), *pos) < rwV3D_Dist2(b.GetPosition(), *pos));
		});
}


void LightManager::SortPointLights()
{
	auto pos = (XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos;
	 auto cameraPos = XMLoadFloat3((XMFLOAT3*)&RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera))->pos);
	auto coors = FindPlayerCoors(-1);
	//auto cameraPos = XMLoadFloat3((XMFLOAT3*)&coors);

	//for (int index = 0; index < mPointLightCount; index++)
	//{
	//	float sqrDistanceToCam = XMVectorGetX(XMVectorMax(g_XMOne, XMVector3Length(cameraPos - XMLoadFloat3(&mPointLightList[index].GetPosition()))));

	//	mPointLightList[index].mPriority = index;
	//}
	//
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

	//std::ofstream myfile;
	//myfile.open("example2.txt");

	//for (size_t i = 0; i < mPointLightCount; i++)
	//{
	//	myfile << mPointLightList[i].mPriority;
	//	myfile << "\n";
	//}

	//myfile.close();
}

void LightManager::AddSpotLight(SpotLight spotlight)
{
	if (mSpotLightCount >= 159)
		return;

	mSpotLightList[mSpotLightCount++] = spotlight;
}

void LightManager::AddSpotLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float angle, float intensity)
{
	if (mSpotLightCount > 159)
		return;

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
	for (size_t i = 0; i < mSpotLightCount; i++)
	{
		mSpotLightList[i].mDrawShadow = true;
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
