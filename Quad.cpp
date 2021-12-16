#include "CommonD.h"
#include "Quad.h"

// Screen quad vertex format
struct ScreenVertex
{
	RwV4d p; // position
	RwV4d t; // texture coordinate
};
struct QuadVertex
{
	RwV4d Position;
	RwV2d TexCoord;
};

IDirect3DVertexDeclaration9* mVertexDeclaration;
IDirect3DIndexBuffer9* mIndexBuffer;
IDirect3DVertexBuffer9* mVertexBuffer;
ScreenVertex svQuad[4];
QuadVertex verts[4] =
{
	{ RwV4d{ -1, 1, 1, 1 }, RwV2d{ 0, 0} },
	{ RwV4d{ 1, 1, 1, 1 }, RwV2d{ 1, 0 } },
	{ RwV4d{ -1, -1, 1, 1 }, RwV2d{ 0, 1 } },
	{ RwV4d{ 1, -1, 1, 1 }, RwV2d{ 1, 1 } }
};


struct QuadVertex2
{
	D3DXVECTOR4 p;
	D3DXVECTOR2 t;
};

#define FVF_QUADVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)


void CreateQuadRender()
{
	D3DVERTEXELEMENT9 declaration[3];
	declaration[0].Stream = 0;
	declaration[0].Offset = 0;
	declaration[0].Type = D3DDECLTYPE_FLOAT4;
	declaration[0].Method = D3DDECLMETHOD_DEFAULT;
	declaration[0].Usage = D3DDECLUSAGE_POSITION;
	declaration[0].UsageIndex = 0;

	declaration[1].Stream = 0;
	declaration[1].Offset = 16;
	declaration[1].Type = D3DDECLTYPE_FLOAT2;
	declaration[1].Method = D3DDECLMETHOD_DEFAULT;
	declaration[1].Usage = D3DDECLUSAGE_TEXCOORD;
	declaration[1].UsageIndex = 0;

	declaration[2].Stream = 0xFF;
	declaration[2].Offset = 0;
	declaration[2].Type = D3DDECLTYPE_UNUSED;
	declaration[2].Method = 0;
	declaration[2].Usage = 0;
	declaration[2].UsageIndex = 0;
	RwD3D9CreateVertexDeclaration(declaration, (void**)&mVertexDeclaration);

	svQuad[0].p = {-1, -1, 1, 1};
	svQuad[0].t = {0, 1};
	svQuad[1].p = {-1, 1, 1, 1};
	svQuad[1].t = {0, 0};
	svQuad[2].p = {1, -1, 1, 1};
	svQuad[2].t = {1, 1};
	svQuad[3].p = {1, 1, 1, 1};
	svQuad[3].t = {1, 0};

	// 
	//RwD3DDevice->CreateVertexBuffer(sizeof(QuadVertex) * 4, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &VertexBuffer, NULL);

	RwUInt32 offset = 0;
	RwD3D9CreateVertexBuffer(sizeof(QuadVertex2), 4 * sizeof(QuadVertex2), (void**)&mVertexBuffer, &offset);


	FLOAT fWidth = (FLOAT)RsGlobal.maximumWidth - 0.5f;
	FLOAT fHeight = (FLOAT)RsGlobal.maximumHeight - 0.5f;

	// Copy the vertices
	QuadVertex2* pVertices = 0;
	mVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0].p = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1].p = D3DXVECTOR4(0.0f, fHeight, 0.0f, 1.0f);
	pVertices[2].p = D3DXVECTOR4(fWidth, 0.0f, 0.0f, 1.0f);
	pVertices[3].p = D3DXVECTOR4(fWidth, fHeight, 0.0f, 1.0f);

	pVertices[0].t = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].t = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[2].t = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[3].t = D3DXVECTOR2(1.0f, 1.0f);
	mVertexBuffer->Unlock();
}


void QuadDestroy()
{

	/*if(IndexBuffer)
		IndexBuffer->Release();
	IndexBuffer = NULL;*/
}

void QuadRender()
{
	// Draw the quad
	RwD3D9SetStreamSource(0, mVertexBuffer, 0, sizeof(QuadVertex2));
	_rwD3D9SetVertexDeclaration(mVertexDeclaration);
	//_rwD3D9SetFVF(FVF_QUADVERTEX);
	_rwD3D9DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//_rwD3D9SetVertexDeclaration(VertexDeclaration);
	//_rwD3D9SetIndices(IndexBuffer);
	//RwD3D9SetStreamSource(0, VertexBuffer, 0, sizeof(QuadVertex));

	////_rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLELIST, 6, verts, sizeof(QuadVertex));

	//_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 6);
}

void DrawScreenQuad()
{
	// QuadRender(); return;

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	FLOAT fWidth = (FLOAT)RsGlobal.maximumWidth - 0.5f;
	FLOAT fHeight = (FLOAT)RsGlobal.maximumHeight - 0.5f;

	struct ScreenVertex
	{
		RwV4dTag       p;
		RwTexCoords t;
	};

	ScreenVertex svQuad[4];

	svQuad[0].p = {-0.5f, -0.5f, 0.5f, 1.0f};
	svQuad[0].t = {0.0f, 0.0f};

	svQuad[1].p = {fWidth, -0.5f, 0.5f, 1.0f};
	svQuad[1].t = {1.0f, 0.0f};

	svQuad[2].p = {-0.5f, fHeight, 0.5f, 1.0f};
	svQuad[2].t = {0.0f, 1.0f};

	svQuad[3].p = {fWidth, fHeight, 0.5f, 1.0f};
	svQuad[3].t = {1.0f, 1.0f};

	_rwD3D9SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	_rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex));
}

void DrawScreenQuad(float x, float y, float width, float height)
{
	struct ScreenVertex
	{
		RwV4dTag    p;
		RwTexCoords t;
	};

	ScreenVertex svQuad[4];
	float z = 0.5;

	svQuad[0].p = {x, y, 0.5f, 1.0f};
	svQuad[0].t = {0.0f, 0.0f};

	svQuad[1].p = {x + width, y, z, 1.0f};
	svQuad[1].t = {1.0f, 0.0f};

	svQuad[2].p = {x, y + height, z, 1.0f};
	svQuad[2].t = {0.0f, 1.0f};

	svQuad[3].p = {x + width, y + width, z, 1.0f};
	svQuad[3].t = {1.0f, 1.0f};

	_rwD3D9SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	_rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex));
}

VertexShader* Quad::mVertexShader = nullptr;
PixelShader* Quad::mPixelShader = nullptr;
VertexBuffer* Quad::mVertexBuffer = nullptr;
RwIndexBuffer* Quad::mIndexBuffer = nullptr;
void* Quad::mVertexDeclQuad = nullptr;

void Quad::Initialize()
{
	CreateQuadRender();
	D3DVERTEXELEMENT9 declaration[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0},
		{0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0},
		D3DDECL_END()
	};

	RwD3D9CreateVertexDeclaration(declaration, &mVertexDeclQuad);

	mVertexShader = new VertexShader();
	mVertexShader->CreateFromBinary("QuadVS");

	mPixelShader = new PixelShader();
	mPixelShader->CreateFromBinary("QuadPS");

	mVertexBuffer = new VertexBuffer();
	mVertexBuffer->Initialize(4, sizeof(QuadVertex), false);
	
	mIndexBuffer = new RwIndexBuffer();
	mIndexBuffer->Initialize(6, false);


	RwUInt32 stride = sizeof(QuadVertex);
	QuadVertex* bufferMem = nullptr;
	mVertexBuffer->Map(stride * 4, (void**)&bufferMem);
	std::copy(verts, verts + 4, bufferMem);
	mVertexBuffer->Unmap();

	static RwUInt16 indices[] = {0, 3, 2, 0, 1, 3};
	RwUInt16* indexBuffer = nullptr;
	mIndexBuffer->Map(sizeof(RwUInt16) * 6, (void**)&indexBuffer);
	std::copy(indices, indices + 6, indexBuffer);
	mIndexBuffer->Unmap();



}

void Quad::Release()
{
	delete mVertexShader;
	delete mPixelShader;
	delete mVertexBuffer;
	delete mIndexBuffer;

	rwD3D9DeleteVertexDeclaration(mVertexDeclQuad);
}

void Quad::Render()
{
	DrawScreenQuad();
	return;

	 _rwD3D9SetVertexShader(0);
	// _rwD3D9SetPixelShader(mPixelShader->GetObject());

	RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, sizeof(QuadVertex));
	 _rwD3D9SetIndices(mIndexBuffer->GetBuffer());
	 _rwD3D9SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	//_rwD3D9SetVertexDeclaration(mVertexDeclQuad);
	_rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	//_rwD3D9SetVertexShader(NULL);
}

void Quad::Render(float x, float y, float width, float height)
{}
