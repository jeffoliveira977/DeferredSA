#pragma once
#include "VertexBuffer.h"
#include "RwIndexBuffer.h"
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
	static void Render();
	static void Render(float x, float y, float width, float height);

private:
	static VertexShader* mVertexShader;
	static PixelShader* mPixelShader;
	static VertexBuffer* mVertexBuffer;
	static RwIndexBuffer* mIndexBuffer;
	static void* mVertexDeclQuad;
};