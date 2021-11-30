#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"
#include "RwIndexBuffer.h"

class RenderableFrustum
{
public:
	float m_nearPlane;
	float m_farPlane;
	RwV2d m_viewWindow;
	RwMatrix* LTM;
	VertexBuffer* m_vertexBuffer;
	RwIndexBuffer* m_indexBuffer;

	RenderableFrustum();
	~RenderableFrustum();

	void InitGraphicsBuffer();
	void RenderFrustum(bool ortho);
};

