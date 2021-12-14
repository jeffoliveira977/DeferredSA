#pragma once
#include "CDrawable.h"

class SkinnedMeshPipeline : public MeshRenderingMode
{
protected:
	void* reflectionParaboloidVS;
	void* reflectionSphericalVS;

	void* PS_deferred;
	void* VS_deferred;

	void* VS_waterReflection;
	void* PS_waterReflection;

	void ShadowRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	static void Hook();
	RxPipeline* Initialize();
};

extern SkinnedMeshPipeline* SkinnedMeshPipe;
