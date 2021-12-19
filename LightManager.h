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
	void AddPointLight(XMFLOAT3 position, XMFLOAT3 direction, XMFLOAT3 color, float radius, float intensity);
	void ClearLights();

	size_t GetSpotLightCount();
	size_t GetPointLightCount();
	SpotLight GetSpotLightAt(int i);
	PointLight GetPointLightAt(int i);
private:

	std::vector<SpotLight> mSpotLightList;
	std::vector<PointLight> mPointLightList;
};

extern LightManager gLightManager;

