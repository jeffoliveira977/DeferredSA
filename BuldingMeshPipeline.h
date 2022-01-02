#pragma once
#include "CDrawable.h"
#include "VertexShader.h"
#include "PixelShader.h"

class BuldingMeshPipeline : virtual public MeshRenderingMode
{
	VertexShader* VS_dualParaboloidMap;
	PixelShader* PS_dualParaboloidMap;

	VertexShader* VS_sphereMap;

	PixelShader* PS_deferred;
	VertexShader* VS_deferred;

	VertexShader* VS_simple;
	PixelShader* PS_simple;

	VertexShader* VS_waterReflection;
	PixelShader* PS_waterReflection;

	VertexShader* mColorVertexShader;
	PixelShader* mColorPixelShader;

	 void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	 void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	 void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	RxPipeline* Initialize();
	static void Hook();
};

extern BuldingMeshPipeline* BuldingMeshPipe;