#include "RenderableFrustum.h"
#include "CCamera.h"
#include "ShaderManager.h"
#include "DynamicVertexBuffer.h"
#include "IndexBufferManager.h"

std::vector<RwUInt16> RenderableFrustum::mIndicesL =
{
	1,  2,  2,  3,  3,  4,  4,  1,
	5,  6,  6,  7,  7,  8,  8,  5,
	9, 10, 10, 11, 11, 12, 12,  9,
	5,  9,  6, 10,  7, 11,  8, 12,
	0,  0
};

std::vector<RwUInt16> RenderableFrustum::mIndicesT =
{
	 5,  6, 10, 10,  9,  5,  6, 7, 11, 11,
	 10, 6,  7,  8, 12, 12, 11, 7,  8,  5,
	 9,  9, 12,  8,  7,  6,  5, 5,  8,  7,
	 9, 10, 11, 11, 12,  9
};

RenderableFrustum::RenderableFrustum()
{
	mVertexBuffer = nullptr;
	mIndexBuffer[0] = nullptr; 
	mIndexBuffer[1] = nullptr;
	mVertexShader = nullptr;
	mPixelShader = nullptr;
}

RenderableFrustum::~RenderableFrustum()
{
	delete mVertexBuffer;
	delete mIndexBuffer[0];
	delete mIndexBuffer[1];
}

void RenderableFrustum::InitGraphicsBuffer()
{
	//mVertexBuffer = new VertexBuffer();
	//mVertexBuffer->Initialize(13, sizeof(Vertex));
	mVertexBuffer = DynamicVertexBuffer::Create(13, sizeof(Vertex));

	/*mIndexBuffer[0] = new RwIndexBuffer();
	mIndexBuffer[0]->Initialize(mIndicesL.size());

	mIndexBuffer[1] = new RwIndexBuffer();
	mIndexBuffer[1]->Initialize(mIndicesT.size());*/

	mIndexBuffer[0] = DynamicIndexBuffer::Create(mIndicesL.size());
	mIndexBuffer[1] = DynamicIndexBuffer::Create(mIndicesT.size());

	mVertexShader = RwCreateCompiledVertexShader("Im3dVS");
	mPixelShader = RwCreateCompiledPixelShader("Im3dPS");
}

void RenderableFrustum::Render(bool ortho)
{
	RwUInt32 numVertices = mVertices.size();

	for(size_t i = 0; i < numVertices; i++)
	{
		if(i < 5)
		{
			RwIm3DVertexSetRGBA(&mVertices[i],
								255, 0, 0, 255);
		}
		else
		{
			RwIm3DVertexSetRGBA(&mVertices[i],
								mColor.x, mColor.y, mColor.z, 255);
		}
	}

	_rwD3D9SetVertexShader(mVertexShader);
	_rwD3D9SetPixelShader(mPixelShader);

	_rwD3D9SetVertexDeclaration(VertexDeclIm3DNoTex);

	float params = 0.0;
	_rwD3D9SetPixelShaderConstant(0, &params, 1);

	_rwD3D9SetVertexShaderConstant(0, &mWorld, 4);
	ShaderContext->SetViewProjectionMatrix(4, true);

	RwUInt32 numIndices = mIndicesL.size();

	RwUInt16* indexData = nullptr;
	mIndexBuffer[0]->Map(numIndices * sizeof(RwUInt16), (void**)&indexData);
	std::copy(mIndicesL.begin(), mIndicesL.end(), indexData);
	mIndexBuffer[0]->Unmap();

	numIndices = mIndicesT.size();
	mIndexBuffer[1]->Map(numIndices * sizeof(RwUInt16), (void**)&indexData);
	std::copy(mIndicesT.begin(), mIndicesT.end(), indexData);
	mIndexBuffer[1]->Unmap();

	Vertex* vertexData = nullptr;
	mVertexBuffer->Map(numVertices * sizeof(Vertex), (void**)&vertexData);
	std::copy(mVertices.begin(), mVertices.end(), vertexData);
	mVertexBuffer->Unmap();
	RwD3D9SetStreamSource(0, mVertexBuffer->GetObject(), 0, sizeof(Vertex));
	_rwD3D9SetIndices(mIndexBuffer[0]->GetObject());
	_rwD3D9DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, numVertices, 0, mIndicesL.size() / 2);

	// Render Filled box

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

	for(size_t i = 5; i < numVertices; i++)
	{
		RwIm3DVertexSetRGBA(&mVertices[i],
							mColor.x, mColor.y, mColor.z, mColor.w);
	}

	mVertexBuffer->Map(numVertices, (void**)&vertexData);
	std::copy(mVertices.begin(), mVertices.end(), vertexData);
	mVertexBuffer->Unmap();
	RwD3D9SetStreamSource(0, mVertexBuffer->GetObject(), 0, sizeof(Vertex));
	_rwD3D9SetIndices(mIndexBuffer[1]->GetObject());
	_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, 0, mIndicesT.size() / 3);
}

void RenderableFrustum::SetProjectionMatrix(XMMATRIX projection, bool ortho)
{
	mProj = projection;

	static float signs[4][2] =
	{
		{ 1,  1}, {-1,  1},
		{-1, -1}, { 1, -1}
	};

	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, mProj);

	float viewLeft, viewRight;
	float nearClip, farClip;

	if(ortho)
	{
		viewLeft = 2.0f / m.m[0][0];
		viewRight = 2.0f / m.m[1][1];
		nearClip = m.m[3][2] / m.m[2][2];
		farClip = nearClip - (1.0f / m.m[2][2]);
	}
	else
	{
		float fov = atan(1.0f / m.m[1][1]);
		float aspect = m.m[1][1] / m.m[0][0];
		viewLeft = fov;
		viewRight = fov / aspect;
		nearClip = m.m[3][2] / m.m[2][2];
		farClip = -m.m[3][2] / (1.0f + m.m[2][2]);
	}

	//PrintMessage("%f %f %f %f", viewLeft, viewRight, nearClip, farClip);

	float depth[3];
	depth[0] = 1.0f;
	depth[1] = nearClip;
	depth[2] = farClip;

	mVertices.clear();
	mVertices.push_back(Vertex(0.0f, 0.0f, 0.0f));

	for(size_t i = 0; i < 3; i++)
	{
		for(size_t j = 1; j < 5; j++)
		{
			if(ortho)
			{
				mVertices.push_back(Vertex(signs[j - 1][0] * viewLeft,
										   signs[j - 1][1] * viewRight,
										   depth[i]));
			}
			else
			{
				mVertices.push_back(Vertex(depth[i] * signs[j - 1][0] * viewLeft,
										   depth[i] * signs[j - 1][1] * viewRight,
										   depth[i]));
			}
		}
	}
}

void RenderableFrustum::SetWorldMatrix(XMMATRIX world)
{
	mWorld = world;
}

void RenderableFrustum::SetColor(XMINT4 color)
{
	mColor = color;
}
