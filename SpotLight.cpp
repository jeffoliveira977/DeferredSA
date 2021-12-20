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

#include "CCamera.h"
#include "CScene.h"
#include "GTADef.h"
void SpotLight::Update()
{
	TheCamera.m_mViewMatrix;

	RwMatrixToXMMATRIX(RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera)));

	float w = static_cast<float>(RsGlobal.maximumWidth);
	float h = static_cast<float>(RsGlobal.maximumHeight);

	XMMATRIX matSpotView;
	XMVECTOR vLookAt = mPosition + mDirection * mRadius;
	XMVECTOR vUp = (XMVectorGetY(mDirection) > 0.9 || XMVectorGetY(mDirection) < -0.9) ? XMVectorSet(0.0f, 0.0f, XMVectorGetY(mDirection), 1.0) : XMVectorSet(0.0f, 1.0f, 0.0f, 1.0);
	XMVECTOR vRight;
	vRight = XMVector3Cross(vUp, mDirection);
	XMVector3Normalize(vRight);
	vUp = XMVector3Cross(mDirection, vRight);
	XMVector3Normalize(vUp);

	XMVECTOR vCenterPos = mPosition + 0.5f * mDirection;
	XMVECTOR vAt = vCenterPos + mDirection;
	XMMATRIX m_LightWorldTransRotate = XMMatrixIdentity();

	m_LightWorldTransRotate.r[0] = vRight;
	m_LightWorldTransRotate.r[1] = vUp;
	m_LightWorldTransRotate.r[2] = mDirection;
	m_LightWorldTransRotate.r[3] = vCenterPos;

	mView = XMMatrixLookAtRH(mPosition, vLookAt, vUp);

	CVector pos = FindPlayerCoors(0);
	XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);

	mProjection = XMMatrixPerspectiveFovRH(cos(DegreesToRadians(mAngle)), 1., 0.01f, mRadius);
	mMatrix = /*m_LightWorldTransRotate **/ mView * mProjection;

	mFrustum.SetMatrix(mMatrix);
}
