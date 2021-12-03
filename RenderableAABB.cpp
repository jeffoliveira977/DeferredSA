#include "RenderableAABB.h"
#include "ShaderManager.h"

RenderableAABB::RenderableAABB()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;
}

RenderableAABB::~RenderableAABB()
{
}

void RenderableAABB::Initialize()
{
	mVertexBuffer = new VertexBuffer();
	mVertexBuffer->Allocate(24, sizeof(_rxD3D9Im3DVertexNoTex));
	mIndexBuffer = new RwIndexBuffer();
	mIndexBuffer->Allocate(36);

	mVertexShader = RwCreateCompiledVertexShader("Im3dVS");
	mPixelShader = RwCreateCompiledPixelShader("Im3dPS");
}

void RenderableAABB::Render(Math::AABB aabb, XMMATRIX matrix)
{
	/*mVertices.clear();

	float w2 = 0.5f * (aabb.Max.x - aabb.Min.x);
	float h2 = 0.5f * (aabb.Max.y - aabb.Min.y);
	float d2 = 0.5f * (aabb.Max.z - aabb.Min.z);

	mVertices.push_back(Vertex(-w2, -h2, -d2, mColor[0]));
	mVertices.push_back(Vertex(-w2, +h2, -d2, mColor[0]));
	mVertices.push_back(Vertex(+w2, +h2, -d2, mColor[0]));
	mVertices.push_back(Vertex(+w2, -h2, -d2, mColor[0]));
	mVertices.push_back(Vertex(-w2, -h2, +d2, mColor[1]));
	mVertices.push_back(Vertex(+w2, -h2, +d2, mColor[1]));
	mVertices.push_back(Vertex(+w2, +h2, +d2, mColor[1]));
	mVertices.push_back(Vertex(-w2, +h2, +d2, mColor[1]));
	mVertices.push_back(Vertex(-w2, +h2, -d2, mColor[2]));
	mVertices.push_back(Vertex(-w2, +h2, +d2, mColor[2]));
	mVertices.push_back(Vertex(+w2, +h2, +d2, mColor[2]));
	mVertices.push_back(Vertex(+w2, +h2, -d2, mColor[2]));
	mVertices.push_back(Vertex(-w2, -h2, -d2, mColor[3]));
	mVertices.push_back(Vertex(+w2, -h2, -d2, mColor[3]));
	mVertices.push_back(Vertex(+w2, -h2, +d2, mColor[3]));
	mVertices.push_back(Vertex(-w2, -h2, +d2, mColor[3]));
	mVertices.push_back(Vertex(-w2, -h2, +d2, mColor[4]));
	mVertices.push_back(Vertex(-w2, +h2, +d2, mColor[4]));
	mVertices.push_back(Vertex(-w2, +h2, -d2, mColor[4]));
	mVertices.push_back(Vertex(-w2, -h2, -d2, mColor[4]));
	mVertices.push_back(Vertex(+w2, -h2, -d2, mColor[5]));
	mVertices.push_back(Vertex(+w2, +h2, -d2, mColor[5]));
	mVertices.push_back(Vertex(+w2, +h2, +d2, mColor[5]));
	mVertices.push_back(Vertex(+w2, -h2, +d2, mColor[5]));*/

	int n = 0;
	for(size_t i = 0; i < 24; i++)
	{		
		RwIm3DVertexSetRGBA(&mVertices[i], mColor[0].x, mColor[0].y, mColor[0].z, mColor[0].w);
	}

	RwImVertexIndex indices[] =
	{
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
	};

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);

	_rwD3D9SetVertexDeclaration(VertexDeclIm3DNoTex);

	mVertexBuffer->Copy(24, mVertices.data());
	mVertexBuffer->Set();

	mIndexBuffer->Copy(36 * sizeof(RwUInt16), indices);
	mIndexBuffer->Set();

	float params = 0.0;
	_rwD3D9SetPixelShaderConstant(0, &params, 1);
	_rwD3D9SetVertexShaderConstant(0, &matrix, 4);
	ShaderContext->SetViewProjectionMatrix(4, true);

	_rwD3D9SetVertexShader(mVertexShader);
	_rwD3D9SetPixelShader(mPixelShader);

	_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 36 / 3);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
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

void RenderableAABB::SetColor(XMINT4 color)
{
	mColor[0] = mColor[1] = mColor[2] = mColor[3] = mColor[4] = mColor[5] = color;
}

void RenderableAABB::SetColor(XMINT4 color1, XMINT4 color2, XMINT4 color3)
{
	mColor[0] = color1;
	mColor[1] = color1;
	mColor[2] = color2;
	mColor[3] = color2;
	mColor[4] = color3;
	mColor[5] = color3;
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