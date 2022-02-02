#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

class SoftParticles
{
public:
	static void Hook();
	static void Initialize();
	static void Release();
private:
	static void Render();
	static void RenderBegin(RwRaster* raster, RwMatrix* transformLTM, unsigned int transformRenderFlags);
	static void RenderEnd();
	static void RenderAddTri(float x1, float y1, float z1,
							 float x2, float y2, float z2, 
							 float x3, float y3, float z3, 
							 float u1, float v1, 
							 float u2, float v2, 
							 float u3, float v3, 
							 int r1, int g1, int b1, int a1, 
							 int r2, int g2, int b2, int a2, 
							 int r3, int g3, int b3, int a3);

	static VertexShader* mVertexShader;
	static PixelShader* mPixelShader;
	static DeferredRenderingEngine::VertexBuffer *mVertexBuffer;
	static void* mVertexDeclColor;
};