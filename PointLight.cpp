#include "PointLight.h"

void PointLight::SetMatrix(XMMATRIX matrix)
{
}

void PointLight::SetColor(XMFLOAT4 color)
{
	mColor = color;
}

void PointLight::SetPosition(XMFLOAT3 position)
{
	mPosition = XMLoadFloat3(&position);
}

void PointLight::SetDirection(XMFLOAT3 direction)
{
	mDirection = XMLoadFloat3(&direction);
}

void PointLight::SetIntensity(float intensity)
{
	mIntensity = intensity;
}
