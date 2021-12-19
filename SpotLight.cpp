#include "SpotLight.h"

void SpotLight::SetMatrix(XMMATRIX matrix)
{
}

void SpotLight::SetColor(XMFLOAT4 color)
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

void SpotLight::Update()
{
	float w = static_cast<float> (RsGlobal.maximumWidth);
	float h = static_cast<float>(RsGlobal.maximumHeight);

	mView = XMMatrixLookAtLH(mDirection, mPosition, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	mProjection = XMMatrixPerspectiveFovLH(mAngle, w / h, 0.01f * mRadius, mRadius);
	mMatrix = mView * mProjection;

	mFrustum.SetMatrix(mMatrix);
}
