#pragma once
#include "CDrawable.h"

class BuldingMeshPipeline : virtual public CDrawable
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

	 void reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	 void deferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	 void forwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	RxPipeline* initGraphics();
	static void hook();
};

extern BuldingMeshPipeline* BuldingMeshPipe;