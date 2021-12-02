#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"
#include "RwIndexBuffer.h"

class RenderableFrustum
{
private:
	float m_nearPlane;
	float m_farPlane;
	RwV2d m_viewWindow;
	RwMatrix* LTM;
	VertexBuffer* m_vertexBuffer;
	RwIndexBuffer* m_indexBuffer;
public:

	RenderableFrustum();
	~RenderableFrustum();

	void SetViewMatrix(RwMatrix* LTM);
	void SetViewWindow(float x, float y); 
	void SetClipPlane(float nearPlane, float farPlane);

	void InitGraphicsBuffer();
	void RenderFrustum(bool ortho);
};

