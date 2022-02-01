#pragma once
#include "CommonD.h"
#include "SpotLight.h"
#include "PointLight.h"

class LightManager
{
public:
	LightManager();

	void Hook();
	void AddSpotLight(SpotLight spotlight);
	void AddPointLight(PointLight pointlight);
	void AddSpotLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float angle, float intensity);
	void AddPointLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float intensity, bool castShadow);
	void ClearLights();
	void SortSpotLights();
	void SortPointLights();
	size_t GetSpotLightCount();
	size_t GetPointLightCount();
	SpotLight* GetSpotLightAt(int i);
	PointLight* GetPointLightAt(int i);
	static PointLight mPointLightList[60];
	static SpotLight mSpotLightList[60];
	static unordered_map<CEntity*, bool> mDontCull;
private:
	static int mSpotLightCount;
	static int mPointLightCount;
};

extern LightManager gLightManager;

