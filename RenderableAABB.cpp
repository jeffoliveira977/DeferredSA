#include "RenderableAABB.h"
#include "ShaderManager.h"
#include "DynamicVertexBuffer.h"

std::list<RwUInt16> RenderableAABB::mIndices =
{
	0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
	8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
};

RenderableAABB::RenderableAABB()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;	
	mVertexShader = nullptr;
	mPixelShader = nullptr;
}

RenderableAABB::~RenderableAABB()
{
	delete mVertexBuffer;
	delete mIndexBuffer;
}

void RenderableAABB::Initialize()
{
	/*mVertexBuffer = new VertexBuffer();
	mVertexBuffer->Initialize(24, sizeof(Vertex));*/
	mVertexBuffer = DynamicVertexBuffer::CreateDynamicVertexBuffer(24, sizeof(Vertex));

	mIndexBuffer = new RwIndexBuffer();
	mIndexBuffer->Initialize(mIndices.size());

	mVertexShader = RwCreateCompiledVertexShader("Im3dVS");
	mPixelShader = RwCreateCompiledPixelShader("Im3dPS");

	RwUInt32 numIndices = mIndices.size();
	RwUInt16* indexData = nullptr;

	mIndexBuffer->Map(numIndices * sizeof(RwUInt16), (void**)&indexData);
	std::copy(mIndices.begin(), mIndices.end(), indexData);
	mIndexBuffer->Unmap();
}

void RenderableAABB::Render()
{
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

	int n = 0;
	for(size_t i = 0; i < mVertices.size(); i++)
	{
		if(i % 5 == 0 && n < 6)
			n++;

		RwIm3DVertexSetRGBA(&mVertices[i], mColor[n].x, mColor[n].y, mColor[n].z, mColor[n].w);
	}

	//PrintMessage("%i", n);

	_rwD3D9SetVertexDeclaration(VertexDeclIm3DNoTex);

	Vertex* vertexData = nullptr;
	mVertexBuffer->Map(mVertices.size() * sizeof(Vertex), (void**)&vertexData);
	std::copy(mVertices.begin(), mVertices.end(), vertexData);
	mVertexBuffer->Unmap();
	RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, sizeof(Vertex));
	_rwD3D9SetIndices(mIndexBuffer->GetBuffer());

	float params = 0.0;
	_rwD3D9SetPixelShaderConstant(0, &params, 1);
	_rwD3D9SetVertexShaderConstant(0, &mWorld, 4);
	ShaderContext->SetViewProjectionMatrix(4, true);

	_rwD3D9SetVertexShader(mVertexShader);
	_rwD3D9SetPixelShader(mPixelShader);

	_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertices.size(), 0, mIndices.size() / 3);
}

void RenderableAABB::SetAABB(Math::AABB aabb)
{
	mVertices.clear();

	float w2 = 0.5f * (aabb.Max.x - aabb.Min.x);
	float h2 = 0.5f * (aabb.Max.y - aabb.Min.y);
	float d2 = 0.5f * (aabb.Max.z - aabb.Min.z);

	mVertices.push_back(Vertex(-w2, -h2, -d2));
	mVertices.push_back(Vertex(-w2, +h2, -d2));
	mVertices.push_back(Vertex(+w2, +h2, -d2));
	mVertices.push_back(Vertex(+w2, -h2, -d2));
	mVertices.push_back(Vertex(-w2, -h2, +d2));
	mVertices.push_back(Vertex(+w2, -h2, +d2));
	mVertices.push_back(Vertex(+w2, +h2, +d2));
	mVertices.push_back(Vertex(-w2, +h2, +d2));
	mVertices.push_back(Vertex(-w2, +h2, -d2));
	mVertices.push_back(Vertex(-w2, +h2, +d2));
	mVertices.push_back(Vertex(+w2, +h2, +d2));
	mVertices.push_back(Vertex(+w2, +h2, -d2));
	mVertices.push_back(Vertex(-w2, -h2, -d2));
	mVertices.push_back(Vertex(+w2, -h2, -d2));
	mVertices.push_back(Vertex(+w2, -h2, +d2));
	mVertices.push_back(Vertex(-w2, -h2, +d2));
	mVertices.push_back(Vertex(-w2, -h2, +d2));
	mVertices.push_back(Vertex(-w2, +h2, +d2));
	mVertices.push_back(Vertex(-w2, +h2, -d2));
	mVertices.push_back(Vertex(-w2, -h2, -d2));
	mVertices.push_back(Vertex(+w2, -h2, -d2));
	mVertices.push_back(Vertex(+w2, +h2, -d2));
	mVertices.push_back(Vertex(+w2, +h2, +d2));
	mVertices.push_back(Vertex(+w2, -h2, +d2));
}

void RenderableAABB::SetWorldMatrix(XMMATRIX world)
{
	mWorld = world;
}

void RenderableAABB::SetColor(XMINT4 color)
{
	mColor[0] = mColor[1] =
	mColor[2] = mColor[3] =
	mColor[4] = mColor[5] = color;
}

void RenderableAABB::SetColor(XMINT4 color1, XMINT4 color2, XMINT4 color3)
{
	mColor[0] = mColor[1] = color1;
	mColor[2] = mColor[3] = color2;
	mColor[4] = mColor[5] = color3;
}

void RenderableAABB::SetColor(XMINT4 color1, XMINT4 color2, XMINT4 color3, XMINT4 color4, XMINT4 color5, XMINT4 color6)
{
	mColor[0] = color1;
	mColor[1] = color2;
	mColor[2] = color3;
	mColor[3] = color4;
	mColor[4] = color5;
	mColor[5] = color6;
}