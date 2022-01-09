#pragma once
#include "CommonD.h"
#include "Frustum.h"

class SpotLight
{
public:

	SpotLight();

	void SetMatrix(XMMATRIX matrix);
	void SetColor(XMFLOAT3 color);
	void SetAngle(float angle);
	void SetPosition(XMFLOAT3 position);
	void SetDirection(XMFLOAT3 direction);
	void SetIntensity(float intensity);
	void SetRadius(float radius);

	Math::Frustum GetFrustum();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjection();
	XMFLOAT3 GetColor();
	XMFLOAT3 GetDirection();
	XMFLOAT3 GetPosition();
	float GetIntensity();
	float GetAngle();
	float GetRadius();

	void Update();
	RwRaster* mColorRaster;
	bool mDrawShadow;
	float mExponent;
	XMMATRIX mMatrix;
private:
	Math::Frustum mFrustum;

	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT3 mColor;
	XMVECTOR mPosition;
	XMVECTOR mDirection;

	float mIntensity;
	float mAngle;
	
	float mRadius;
};

