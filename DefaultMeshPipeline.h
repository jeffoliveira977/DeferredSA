#pragma once
#include "CDrawable.h"

class DefaultMeshPipeline : virtual public MeshRenderingMode
{
	void* reflectionParaboloidVS;
	void* reflectionSphericalVS;

	void* PS_deferred;
	void* VS_deferred;

	void* VS_simple;
	void* PS_simple;

	void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	static void Hook();
	RwBool Initialize();
};

extern DefaultMeshPipeline* DefaultMeshPipe;