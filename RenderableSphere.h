#pragma once

#include "CommonD.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BoundingSphere.h"
#include "PixelShader.h"
#include "VertexShader.h"

class RenderableSphere
{
	struct Vertex
	{
		float x, y, z;
		int32_t color;

		Vertex(){};
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
	VertexShader* mVertexShader;
	PixelShader* mPixelShader;
	DeferredRenderingEngine::VertexBuffer* mVertexBuffer;
	DeferredRenderingEngine::IndexBuffer* mIndexBuffer;
	XMINT4 mColor;
	std::vector<Vertex> mVertices;
	std::vector<RwUInt16> mIndices;
	Math::BoundingSphere mSphere;
public:
	RenderableSphere();
	~RenderableSphere();

	void Initialize(int slices, int stacks);
	void Render();
	void SetSphere(Math::BoundingSphere sphere);
	void SetWorldMatrix(XMMATRIX world);
	void SetColor(XMINT4 color);
};

