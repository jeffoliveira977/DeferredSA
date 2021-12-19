#include "PointLight.h"

void PointLight::SetMatrix(XMMATRIX matrix)
{
}

void PointLight::SetColor(XMFLOAT3 color)
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

void PointLight::SetRadius(float radius)
{
	mRadius = radius;
}

XMMATRIX PointLight::GetViewMatrix()
{
	return mView;
}

XMMATRIX PointLight::GetProjection()
{
	return mProjection;
}

XMFLOAT3 PointLight::GetColor()
{
	return mColor;
}

XMFLOAT3 PointLight::GetDirection()
{
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, mDirection);
	return direction;
}

XMFLOAT3 PointLight::GetPosition()
{
	XMFLOAT3 position;
	XMStoreFloat3(&position, mPosition);
	return position;
}

float PointLight::GetIntensity()
{
	return mIntensity;
}

float PointLight::GetRadius()
{
	return mRadius;
}