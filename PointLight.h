#pragma once
#include "CommonD.h"
#include "Frustum.h"
#include "BoundingSphere.h"
class PointLight
{
public:
	void SetMatrix(XMMATRIX matrix);
	void SetColor(XMFLOAT3 color);
	void SetPosition(XMFLOAT3 position);
	void SetDirection(XMFLOAT3 direction);
	void SetIntensity(float intensity);
	void SetRadius(float radius);

	Math::Frustum GetFrustum(int i);
	XMMATRIX GetViewMatrix(int i);
	XMMATRIX GetProjection();
	XMFLOAT3 GetColor();
	XMFLOAT3 GetDirection();
	 XMFLOAT3 GetPosition();
	float GetIntensity();
	float GetRadius();
	XMMATRIX GetWorld();
	void Update();
	float mPriority;
private:
	Math::BoundingSphere mSphere;

	XMMATRIX mWorld;
	XMMATRIX mMatrix;
	XMMATRIX mView[6];
	XMMATRIX mProjection;
	XMFLOAT3 mColor;
	XMVECTOR mPosition;
	XMVECTOR mDirection;
	Math::Frustum mFrustum[6];
	float mRadius;
	float mIntensity;
};

