#include "RenderableFrustum.h"
#include "CCamera.h"
#include "ShaderManager.h"

RenderableFrustum::RenderableFrustum()
{
	m_nearPlane = 0.0;
	m_farPlane = 0.0;
	m_viewWindow.x = m_viewWindow.y = 0.0;
	LTM = nullptr;
}

RenderableFrustum::~RenderableFrustum()
{
}

void RenderableFrustum::SetViewMatrix(RwMatrix* LTM)
{
	this->LTM = LTM;
}

void RenderableFrustum::SetViewWindow(float x, float y)
{
	m_viewWindow.x = x;
	m_viewWindow.y = y;
}

void RenderableFrustum::SetClipPlane(float nearPlane, float farPlane)
{
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
}

void RenderableFrustum::InitGraphicsBuffer()
{
	mVertexBuffer = new VertexBuffer();
	mVertexBuffer->Initialize(13, sizeof(Vertex));
	
	mIndexBuffer = new RwIndexBuffer();
	mIndexBuffer->Initialize(36);

	mVertexShader = RwCreateCompiledVertexShader("Im3dVS");
	mPixelShader = RwCreateCompiledPixelShader("Im3dPS");
}

void RenderableFrustum::RenderFrustum(bool ortho)
{
	static RwRGBA red = {255,   0, 0, 255};

	/* Line index */
	std::vector<RwImVertexIndex> indicesL =
	{
		1,  2,  2,  3,  3,  4,  4,  1,
		5,  6,  6,  7,  7,  8,  8,  5,
		9, 10, 10, 11, 11, 12, 12,  9,
		5,  9,  6, 10,  7, 11,  8, 12,
		0,  0
	};

	/* Triangle index */
	std::vector<RwImVertexIndex> indicesT =
	{
		 5,  6, 10, 10,  9,  5,  6, 7, 11, 11, 
		 10, 6,  7,  8, 12, 12, 11, 7,  8,  5,  
		 9,  9, 12,  8,  7,  6,  5, 5,  8,  7,
		 9, 10, 11, 11, 12,  9
	};

	RwReal signs[4][2] =
	{
		{ 1,  1}, {-1,  1},
		{-1, -1}, { 1, -1}
	};

	RwInt32 i = 0;
	RwInt32 j = 0;

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

	//PrintMessage("%f %f %f %f", m.m[3][2], m.m[2][2], nearClip,  farClip);
	
	RwReal depth[3];
	depth[0] = 1.0f;
	depth[1] = nearClip;
	depth[2] = farClip;

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(0.0f, 0.0f, 0.0f));

	for(i = 0; i < 3; i++)
	{
		for(j = 1; j < 5; j++)
		{
			if(ortho)
			{
				vertices.push_back(Vertex(signs[j - 1][0] * viewLeft,
										   signs[j - 1][1] * viewRight, depth[i]));
			}		
			else
			{
				vertices.push_back(Vertex(depth[i] * signs[j - 1][0] * viewLeft,
										   depth[i] * signs[j - 1][1] * viewRight, depth[i]));
			}
		}
	}

	RwUInt32 numVertices = vertices.size();

	for(i = 0; i < numVertices; i++)
	{
		if(i < 5)
		{
			RwIm3DVertexSetRGBA(&vertices[i],
								red.red, red.green, red.blue, red.alpha);
		}
		else
		{
			RwIm3DVertexSetRGBA(&vertices[i],
								mColor.x, mColor.y, mColor.z, 255);
		}
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);

	_rwD3D9SetVertexShader(mVertexShader);
	_rwD3D9SetPixelShader(mPixelShader);

	_rwD3D9SetVertexDeclaration(VertexDeclIm3DNoTex);

	float params = 0.0;
	_rwD3D9SetPixelShaderConstant(0, &params, 1);
	_rwD3D9SetVertexShaderConstant(0, LTM, 4);
	ShaderContext->SetViewProjectionMatrix(4, true);

	RwUInt32 numIndices = indicesL.size();

	Vertex* bufferMem = nullptr;
	mVertexBuffer->Map(numVertices * sizeof(Vertex), (void**)&bufferMem);
	std::copy(vertices.begin(), vertices.end(), bufferMem);
	mVertexBuffer->Unmap();

	RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, sizeof(Vertex));

	RwImVertexIndex* bufferIndices = nullptr;
	mIndexBuffer->Map(numIndices * sizeof(RwUInt16), (void**)&bufferIndices);
	std::copy(indicesL.begin(), indicesL.end(), bufferIndices);
	mIndexBuffer->Unmap();
	_rwD3D9SetIndices(mIndexBuffer->GetBuffer());

	_rwD3D9DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, numVertices, 0, numIndices / 2);

	// Render Filled box

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

	for(i = 5; i < numVertices; i++)
	{
		RwIm3DVertexSetRGBA(&vertices[i],
							mColor.x, mColor.y, mColor.z, mColor.w);
	}

	mVertexBuffer->Map(numVertices, (void**)&bufferMem);
	std::copy(vertices.begin(), vertices.end(), bufferMem);
	mVertexBuffer->Unmap();
	RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, sizeof(Vertex));
	
	numIndices = indicesT.size();
	mIndexBuffer->Map(numIndices * sizeof(RwUInt16), (void**)&bufferIndices);
	std::copy(indicesT.begin(), indicesT.end(), bufferIndices);
	mIndexBuffer->Unmap();
	_rwD3D9SetIndices(mIndexBuffer->GetBuffer());
	
	_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, 0, numIndices / 3);
	
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
}

void RenderableFrustum::SetColor(XMINT4 color)
{
	mColor = color;
}
