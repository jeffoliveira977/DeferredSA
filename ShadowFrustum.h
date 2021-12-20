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
	void DirectionalLightTransform(RwCamera* mainCam, CVector lightDir, int shadowCascade);

	
	XMMATRIX m_LightSpaceMatrix;

	bool mStabilizeCascades;

	int CascadeCount;
	int ShadowSize;
	float FilterSize;
	ShadowPartitionMode m_partitionMode;

	struct
	{
		XMMATRIX mLightViewMatrix;
		XMMATRIX mLightOrthoMatrix;
		Math::Frustum mFrustumCulling;
	}Desc[4];

protected:
	XMVECTOR  CalculateCameraPos(RwCamera* mainCam, const RW::V3d& lightDir, int shadowCascade);

	float MinDistance = 20.0;
	float MaxDistance = 300.0;
	float PSSMLambda = 0.0100f;
	
	float BiasCoefficients[4];
	float mDistanceCoefficients[4];
	float m_fShadowDistances[5];
};

