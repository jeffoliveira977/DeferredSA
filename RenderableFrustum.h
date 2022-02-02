#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Frustum.h"

class RenderableFrustum
{
private:
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
	float m_nearPlane;
	float m_farPlane;
	RwV2d m_viewWindow;
	RwMatrix* LTM;
	DeferredRenderingEngine::VertexBuffer* mVertexBuffer;
	DeferredRenderingEngine::IndexBuffer* mIndexBuffer[2];
	void* mVertexShader;
	void* mPixelShader;
	XMINT4 mColor;
	static std::vector<RwUInt16> mIndicesL;
	static std::vector<RwUInt16> mIndicesT;
	std::vector<Vertex> mVertices;
	XMMATRIX mWorld;
	XMMATRIX mProj;
public:
	RenderableFrustum();
	~RenderableFrustum();

	void InitGraphicsBuffer();
	void Render(bool ortho);
	void SetColor(XMINT4 color);
	void SetWorldMatrix(XMMATRIX world);
	void SetProjectionMatrix(XMMATRIX projection, bool ortho);
};

