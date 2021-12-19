#pragma once
#include "CommonD.h"

class PointLight
{
public:
	void SetMatrix(XMMATRIX matrix);
	void SetColor(XMFLOAT3 color);
	void SetPosition(XMFLOAT3 position);
	void SetDirection(XMFLOAT3 direction);
	void SetIntensity(float intensity);
	void SetRadius(float radius);

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjection();
	XMFLOAT3 GetColor();
	XMFLOAT3 GetDirection();
	XMFLOAT3 GetPosition();
	float GetIntensity();
	float GetRadius();

	void Update();
private:
	XMMATRIX mMatrix;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT3 mColor;
	XMVECTOR mPosition;
	XMVECTOR mDirection;

	float mRadius;
	float mIntensity;
};

