#pragma once
#include "CommonD.h"
#include "Frustum.h"
class SpotLight
{
public:
	void SetMatrix(XMMATRIX matrix);
	void SetColor(XMFLOAT4 color);
	void SetAngle(float angle);
	void SetPosition(XMFLOAT3 position);
	void SetDirection(XMFLOAT3 direction);
	void SetIntensity(float intensity);
	void Update();
private:
	Math::Frustum mFrustum;
	XMMATRIX mMatrix;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 mColor;
	XMVECTOR mPosition;
	XMVECTOR mDirection;

	float mIntensity;
	float mAngle;
	float mRadius;
};

