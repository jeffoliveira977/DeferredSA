#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

void CreateQuadRender();
void QuadDestroy();
void QuadRender();
void DrawScreenQuad();
void DrawScreenQuad(float x, float y, float width, float height);

class Quad
{
public:
	static void Initialize();
	static void Release();
	static void Render(bool useVS=true);
	static void Render(float x, float y, float width, float height);

private:
	static VertexShader* mVertexShader;
	static PixelShader* mPixelShader;
	static DeferredRenderingEngine::VertexBuffer* mVertexBuffer;
	static DeferredRenderingEngine::IndexBuffer* mIndexBuffer;
	static void* mVertexDeclQuad;
};