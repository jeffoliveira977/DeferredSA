#pragma once

#include "CommonD.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "AABB.h"

class RenderableAABB
{
	struct Vertex
	{
		float x, y, z;
		int32_t color;

		Vertex(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		};

		Vertex(float x, float y, float z, XMINT4 color)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->color = ((color.w << 24) | ((color.x) << 16) |
						  ((color.y) << 8) | ((color.z)));
		}
	};

	XMMATRIX mWorld;
	void* mVertexShader;
	void* mPixelShader;
	DeferredRenderingEngine::VertexBuffer* mVertexBuffer;
	DeferredRenderingEngine::IndexBuffer* mIndexBuffer;
	XMINT4 mColor[6];
	std::vector<Vertex> mVertices;
	static std::list<RwUInt16> mIndices;
public:
	RenderableAABB();
	~RenderableAABB();

	void Initialize();
	void Render();
	void SetAABB(Math::AABB aabb);
	void SetWorldMatrix(XMMATRIX world);
	void SetColor(XMINT4 color);
	void SetColor(XMINT4 color, XMINT4 color2, XMINT4 color3);
	void SetColor(XMINT4 color, XMINT4 color2, XMINT4 color3, XMINT4 color4, XMINT4 color5, XMINT4 color6);
};

