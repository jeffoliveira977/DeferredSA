#pragma once
#include "CDrawable.h"

class DefaultMeshPipeline : virtual public CDrawable
{
	void* reflectionParaboloidVS;
	void* reflectionSphericalVS;

	void* PS_deferred;
	void* VS_deferred;

	void* VS_simple;
	void* PS_simple;

	void reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void deferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void forwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	static void hook();
	RwBool initGraphics();
};

extern DefaultMeshPipeline* DefaultMeshPipe;