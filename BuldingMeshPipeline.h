#pragma once
#include "CDrawable.h"

class BuldingMeshPipeline : virtual public MeshRenderingMode
{
	void* VS_dualParaboloidMap;
	void* PS_dualParaboloidMap;

	void* VS_sphereMap; 

	void* PS_deferred;
	void* VS_deferred;

	void* VS_simple;
	void* PS_simple;

	void* VS_waterReflection;
	void* PS_waterReflection;

	 void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	 void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	 void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	RxPipeline* initGraphics();
	static void hook();
};

extern BuldingMeshPipeline* BuldingMeshPipe;