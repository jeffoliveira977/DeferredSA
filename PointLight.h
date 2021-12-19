#pragma once
#include "CommonD.h"

class PointLight
{
public:
	void SetMatrix(XMMATRIX matrix);
	void SetColor(XMFLOAT4 color);
	void SetPosition(XMFLOAT3 position);
	void SetDirection(XMFLOAT3 direction);
	void SetIntensity(float intensity);
	void Update();
private:
	XMMATRIX mMatrix;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 mColor;
	XMVECTOR mPosition;
	XMVECTOR mDirection;

	float mIntensity;
};

