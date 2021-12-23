#include "PointLight.h"

 //RwRaster* PointLight::mShadowRaster[60];

PointLight::PointLight()
{
	
	//mShadowRaster = nullptr;
}

PointLight::PointLight(int width, int height)
{
	//mShadowRaster = RwD3D9RasterCreate(width, height, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);
	// rwD3D9CubeRasterCreate(mShadowRaster, D3DFMT_G32R32F, 1);
}

PointLight::~PointLight()
{
	//if (mShadowRaster == nullptr)
	//	return;

	//RwRasterDestroy(mShadowRaster);

	//std::ofstream myfile;
	//myfile.open("example2.txt");
	//myfile << "Writing this to a file.\n";
	//myfile << "Writing this to a file.\n";
	//myfile << "Writing this to a file.\n";
	//myfile << "Writing this to a file.\n";
	//myfile.close();
}

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
	mRadius = max(1.0f, mRadius);

	XMStoreFloat3(&mSphere.Center, mDirection);
	mSphere.Radius = mRadius;

	XMMATRIX scaling = XMMatrixScaling(mRadius, mRadius, mRadius);
	XMMATRIX translation = XMMatrixTranslationFromVector(mDirection);

	mWorld = scaling * translation;
	mProjection = XMMatrixPerspectiveFovRH(XMConvertToRadians(90.0f), 1.0f, 0.1f, mRadius);

	XMVECTOR lookAt;
	XMVECTOR up;


	for (size_t i = 0; i < 6; i++)
	{
	//	mView[i] = XMMatrixLookAtRH(-mPosition, (mPosition + DXCubeForward[i]), DXCubeUp[i]);
		switch (static_cast<D3DCUBEMAP_FACES>(i))
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
			lookAt = g_XMIdentityR0;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + g_XMIdentityR0, -g_XMIdentityR1);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			lookAt = -g_XMIdentityR0;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + -g_XMIdentityR0, -g_XMIdentityR1);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			lookAt = g_XMIdentityR1;
			up = -g_XMIdentityR2;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + -g_XMIdentityR1, -g_XMIdentityR2);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			lookAt = -g_XMIdentityR1;
			up = g_XMIdentityR2;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + g_XMIdentityR1, g_XMIdentityR2);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			lookAt = g_XMIdentityR2;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + g_XMIdentityR2, -g_XMIdentityR1);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			lookAt = -g_XMIdentityR2;
			up = g_XMIdentityR1;
			mView[i] = XMMatrixLookAtRH(mPosition, mPosition + -g_XMIdentityR2, -g_XMIdentityR1);
			break;
		}
		//mView[i] = XMMatrixLookAtRH(mPosition, lookAt, up);
		mMatrix = mView[i] * mProjection;
		mFrustum[i].SetMatrix(mMatrix);
	}
}
