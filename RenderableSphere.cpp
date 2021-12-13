#include "RenderableSphere.h"
#include "ShaderManager.h"
#include "PixelShaderConstant.h"
#include "VertexShaderConstant.h"

RenderableSphere::RenderableSphere()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;
	mVertexShader = nullptr;
	mPixelShader = nullptr;
}

RenderableSphere::~RenderableSphere()
{
	delete mVertexBuffer; 
	delete mIndexBuffer;
	delete mVertexShader;
	delete mPixelShader;
}

void RenderableSphere::Initialize(int slices, int stacks)
{
	int numVertices = 2 + slices * (stacks - 1);
	int numIndices = 2 * slices + (stacks - 2) * (2 * slices);

	mVertexBuffer = new VertexBuffer();
	mVertexBuffer->Initialize(numVertices, sizeof(Vertex));

	mIndexBuffer = new RwIndexBuffer();
	mIndexBuffer->Initialize(numIndices * 3);

	mVertexShader = new VertexShader();
	mVertexShader->CreateFromBinary("Im3dVS");

	mPixelShader = new PixelShader();
	mPixelShader->CreateFromBinary("Im3dPS");

	mVertices.resize(numVertices);
	mIndices.resize(numIndices * 3);

	float phiStep, phiStart;
	float thetaStep, theta;
	RwUInt32 vertexCount, indicesCount;

	phiStep = -2 * XM_PI / slices;
	phiStart = XM_PI / 2;

	thetaStep = XM_PI / stacks;
	theta = thetaStep;

	vertexCount = 0;
	indicesCount = 0;

	mVertices[vertexCount] = {0.0f, 0.0f, 1.0f};
	vertexCount++;

	int i, j;
	for(i = 0; i < stacks - 1; i++)
	{
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for(j = 0; j < slices; j++)
		{
			float x = sinTheta * cosf(phiStart);
			float y = sinTheta * sinf(phiStart);

			mVertices[vertexCount] = {x, y, cosTheta};

			vertexCount++;

			phiStart += phiStep;

			if(j > 0)
			{
				if(i == 0)
				{
					mIndices[indicesCount++] = 0;
					mIndices[indicesCount++] = j + 1;
					mIndices[indicesCount++] = j;
				}
				else
				{
					mIndices[indicesCount++] = (i - 1) * slices + j;
					mIndices[indicesCount++] = (i - 1) * slices + j + 1;
					mIndices[indicesCount++] = i * slices + j;

					mIndices[indicesCount++] = (i - 1) * slices + j + 1;
					mIndices[indicesCount++] = i * slices + j + 1;
					mIndices[indicesCount++] = i * slices + j;
				}
			}
		}

		theta += thetaStep;

		if(i == 0)
		{
			mIndices[indicesCount++] = 0;
			mIndices[indicesCount++] = 1;
			mIndices[indicesCount++] = j;
		}
		else
		{
			mIndices[indicesCount++] = (i - 1) * slices + j;
			mIndices[indicesCount++] = (i - 1) * slices + 1;
			mIndices[indicesCount++] = i * slices + j;

			mIndices[indicesCount++] = (i - 1) * slices + 1;
			mIndices[indicesCount++] = i * slices + 1;
			mIndices[indicesCount++] = i * slices + j;
		}
	}

	mVertices[vertexCount] = {0.0f, 0.0f, -1.0f};

	for(j = 1; j < slices; j++)
	{
		mIndices[indicesCount++] = (i - 1) * slices + j;
		mIndices[indicesCount++] = (i - 1) * slices + j + 1;
		mIndices[indicesCount++] = vertexCount;
	}

	mIndices[indicesCount++] = (i - 1) * slices + j;
	mIndices[indicesCount++] = (i - 1) * slices + 1;
	mIndices[indicesCount++] = vertexCount;

	Vertex* vertexData = nullptr;
	mVertexBuffer->Map(mVertices.size() * sizeof(Vertex), (void**)&vertexData);
	std::copy(mVertices.begin(), mVertices.end(), vertexData);
	mVertexBuffer->Unmap();

	RwUInt16* indexData = nullptr;
	mIndexBuffer->Map(mIndices.size() * sizeof(RwUInt16), (void**)&indexData);
	std::copy(mIndices.begin(), mIndices.end(), indexData);
	mIndexBuffer->Unmap();

}

void RenderableSphere::Render()
{
	if(mVertexBuffer == nullptr || mVertices.size() == 0)
		return;

	RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, sizeof(Vertex));
	_rwD3D9SetIndices(mIndexBuffer->GetBuffer());

	XMMATRIX translation = XMMatrixTranslation(mSphere.Center.x, mSphere.Center.y, mSphere.Center.z);
	XMMATRIX scale = XMMatrixScaling(mSphere.Radius, mSphere.Radius, mSphere.Radius);

	PixelShaderConstant::SetFloat(0, 0.0);
	VertexShaderConstant::SetMatrix(0, mWorld * translation * scale);
	//ShaderContext->SetViewProjectionMatrix(4, true);

	_rwD3D9SetVertexDeclaration(VertexDeclIm3DNoTex);

	_rwD3D9SetVertexShader(mVertexShader->GetObject());
	_rwD3D9SetPixelShader(mPixelShader->GetObject());

	_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertices.size(), 0, mIndices.size() / 3);
}

void RenderableSphere::SetSphere(Math::BoundingSphere sphere)
{
	mSphere = sphere;
}

void RenderableSphere::SetWorldMatrix(XMMATRIX world)
{
	mWorld = world;
}

void RenderableSphere::SetColor(XMINT4 color)
{
	if(mVertexBuffer == nullptr || mVertices.size() == 0)
		return;

	if(XMVector4NotEqual(XMLoadSInt4(&mColor), XMLoadSInt4(&color)))
	{
		for(size_t i = 0; i < mVertices.size(); i++)
		{
			RwIm3DVertexSetRGBA(&mVertices[i],
								color.x, color.y,
								color.z, color.w);
		}

		Vertex* vertexData = nullptr;
		mVertexBuffer->Map(mVertices.size() * sizeof(Vertex), (void**)&vertexData);
		std::copy(mVertices.begin(), mVertices.end(), vertexData);
		mVertexBuffer->Unmap();
		RwUInt16* indexData = nullptr;
		mIndexBuffer->Map(mIndices.size() * sizeof(RwUInt16), (void**)&indexData);
		std::copy(mIndices.begin(), mIndices.end(), indexData);
		mIndexBuffer->Unmap();
		mColor = color;
	}
}
