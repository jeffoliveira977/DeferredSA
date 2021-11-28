#pragma once
#include "CommonD.h"

class SoftParticles
{
	void* VS_softParticles;
	void* PS_softParticles;
public:
	SoftParticles();
	~SoftParticles();

	void hook();
	void initGraphicsBuffer();
	void SetupParams();
	static void RenderBegin(RwRaster* raster, RwMatrix* transformLTM, unsigned int transformRenderFlags);
	static void RenderEnd();
	static void RenderAddTri(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float u1, float v1, float u2, float v2, float u3, float v3, int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3);

};

extern SoftParticles *SoftParticlesContext;
