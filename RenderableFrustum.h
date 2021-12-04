#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"
#include "RwIndexBuffer.h"
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
	VertexBuffer* mVertexBuffer;
	RwIndexBuffer* mIndexBuffer;
	void* mVertexShader;
	void* mPixelShader;
	XMINT4 mColor;
public:
	XMMATRIX mView;
	XMMATRIX mProj;
	Math::Frustum m_frustum;
	RenderableFrustum();
	~RenderableFrustum();

	void SetViewMatrix(RwMatrix* LTM);
	void SetViewWindow(float x, float y); 
	void SetClipPlane(float nearPlane, float farPlane);

	void InitGraphicsBuffer();
	void RenderFrustum(bool ortho);
	void SetColor(XMINT4 color);
};

