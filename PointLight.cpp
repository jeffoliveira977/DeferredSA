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

Math::Frustum PointLight::GetFrustum(int i)
{
	return mFrustum[i];
}

XMMATRIX PointLight::GetViewMatrix(int i)
{
	return mView[i];
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

XMMATRIX PointLight::GetWorld()
{
	return mWorld;
}

 const XMVECTOR DXCubeForward[6] =
{
	{1, 0, 0, 0},
{-1, 0, 0, 0 },
	{0, 1, 0, 0},
	{0, -1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, -1, 0},
};

 const XMVECTOR DXCubeUp[6] =
{
	{0, 1, 0, 0 },
	{0, 1, 0, 0},
	{0, 0, -1, 0 },
	{0, 0, 1, 0 },
	{0, 1, 0, 0 },
	{0, 1, 0, 0 },
};

void PointLight::Update()
{
	XMStoreFloat3(&mSphere.Center, mPosition);
	mSphere.Radius = mRadius;

	XMMATRIX scaling = XMMatrixScaling(mRadius, mRadius, mRadius);
	XMMATRIX translation = XMMatrixTranslationFromVector(mPosition);

	mWorld = scaling * translation;
	mProjection = XMMatrixPerspectiveFovRH(XM_PI / 2.0f, 1.0, 0.1f, mRadius);

	XMVECTOR lookAt;
	XMVECTOR up;

	for (size_t i = 0; i < 6; i++)
	{
		mView[i] = XMMatrixLookAtRH(mPosition, (mPosition + DXCubeForward[i]), DXCubeUp[i]);
		mMatrix = mView[i] * mProjection;
		mFrustum[i].SetMatrix(mMatrix);
	}
}
