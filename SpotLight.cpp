#include "SpotLight.h"

SpotLight::SpotLight()
{
}

void SpotLight::SetMatrix(XMMATRIX matrix)
{
}

void SpotLight::SetColor(XMFLOAT3 color)
{
	mColor = color;
}

void SpotLight::SetAngle(float angle)
{
	mAngle = angle;
}

void SpotLight::SetPosition(XMFLOAT3 position)
{
	mPosition = XMLoadFloat3(&position);
}

void SpotLight::SetDirection(XMFLOAT3 direction)
{
	mDirection = XMLoadFloat3(&direction);
}

void SpotLight::SetIntensity(float intensity)
{
	mIntensity = intensity;
}

void SpotLight::SetRadius(float radius)
{
	mRadius = radius;
}

Math::Frustum SpotLight::GetFrustum()
{
	return mFrustum;
}

XMMATRIX SpotLight::GetViewMatrix()
{
	return mView;
}

XMMATRIX SpotLight::GetProjection()
{
	return mProjection;
}

XMFLOAT3 SpotLight::GetColor()
{
	return mColor;
}

XMFLOAT3 SpotLight::GetDirection()
{
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, mDirection);
	return direction;
}

XMFLOAT3 SpotLight::GetPosition()
{
	XMFLOAT3 position;
	XMStoreFloat3(&position, mPosition);
	return position;
}

float SpotLight::GetIntensity()
{
	return mIntensity;
}

float SpotLight::GetAngle()
{
	return mAngle;
}

float SpotLight::GetRadius()
{
	return mRadius;
}

void SpotLight::Update()
{
	XMVECTOR right, lookAt, up;
	
	up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	/*if (XMVectorGetY(mDirection) > 0.9f || XMVectorGetY(mDirection) < -0.9f)
		up = XMVectorSet(0.0f, 0.0f, XMVectorGetY(mDirection), 1.0f);

	right = XMVector3Cross(up, mDirection);
	right = XMVector3Normalize(right);
	up = XMVector3Cross(mDirection, right);
	up = XMVector3Normalize(up);*/

	lookAt = mPosition + mDirection * mRadius;
	mView = XMMatrixLookAtLH(mPosition, lookAt, up);
	mProjection = XMMatrixPerspectiveFovLH(cos(XMConvertToRadians(mAngle)), 1.0f, 1.f, mRadius);
	mMatrix = mView * mProjection;

	mFrustum.SetMatrix(mMatrix);
}
