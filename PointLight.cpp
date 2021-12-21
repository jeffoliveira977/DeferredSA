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

void PointLight::Update()
{
	XMStoreFloat3(&mSphere.Center, mPosition);
	mSphere.Radius = mRadius;

	XMMATRIX scaling = XMMatrixScaling(mRadius, mRadius, mRadius);
	XMMATRIX translation = XMMatrixTranslationFromVector(mPosition);

	mWorld = scaling * translation;

	mProjection = XMMatrixPerspectiveFovRH(XM_PI / 2.0f, 1.0f, 0.1f, mRadius);

	XMVECTOR lookAt;
	XMVECTOR up;

	for (size_t i = 0; i < 6; i++)
	{
		switch (static_cast<D3DCUBEMAP_FACES>(i))
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
			lookAt = g_XMIdentityR0;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition+ g_XMIdentityR0, g_XMIdentityR2);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			lookAt = -g_XMIdentityR0;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition - g_XMIdentityR0, g_XMIdentityR2);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			lookAt = g_XMIdentityR1;
			up = -g_XMIdentityR2;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + g_XMIdentityR1, g_XMIdentityR2);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			lookAt = -g_XMIdentityR1;
			up = g_XMIdentityR2;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition - g_XMIdentityR1, g_XMIdentityR2);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			lookAt = g_XMIdentityR2;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + g_XMIdentityR2, g_XMIdentityR0);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			lookAt = -g_XMIdentityR2;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition - g_XMIdentityR2, g_XMIdentityR0);
			break;
		}
		//mView[i] = XMMatrixLookAtRH(mPosition, lookAt, up);
		mMatrix = mView[i] * mProjection;
		mFrustum[i].SetMatrix(mMatrix);
	}
}
