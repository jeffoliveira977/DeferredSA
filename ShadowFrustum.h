#pragma once
#include "CommonD.h"
#include "RwMath.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "Frustum.h"
#include "AABB.h"

enum ShadowPartitionMode
{
	PartitionMode_Manual,
	PartitionMode_Logarithmic,
	PartitionMode_PSSM
};

class ShadowFrustum
{
public:
	void CalculateShadowDistances(const RwReal fNear, const RwReal fFar);
	void DirectionalLightTransform(RwCamera* mainCam, const RW::V3d& lightDir, int shadowCascade);

	
	XMMATRIX m_LightSpaceMatrix;



	int CascadeCount;
	int ShadowSize;
	float FilterSize;
	ShadowPartitionMode m_partitionMode;
	XMVECTOR m_planes[4][6];
	struct
	{
		XMMATRIX lightViewMatrix;
		XMMATRIX lightOrthoMatrix;
		XMMATRIX g_mLightViewProj;
		Math::AABB m_AABB;
		XMFLOAT3 FrustumCenter;
		Math::Frustum m_FrustumCulling;
		float NearClip;
		float FarClip;
		XMFLOAT2 ViewWindow;
	}Desc[4];
protected:
	XMVECTOR  CalculateCameraPos(RwCamera* mainCam, const RW::V3d& lightDir, int shadowCascade);

	float MinDistance = 20.0;
	float MaxDistance = 300.0;
	float PSSMLambda = 0.0100f;
	
	float BiasCoefficients[4];
	float DistanceCoefficients[4];
	float m_fShadowDistances[5];
};

