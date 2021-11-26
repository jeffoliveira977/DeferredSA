#pragma once
#include "CDrawable.h"

class SkinnedMeshPipeline : public CDrawable
{
protected:
	void* reflectionParaboloidVS;
	void* reflectionSphericalVS;

	void* PS_deferred;
	void* VS_deferred;

	void* VS_waterReflection;
	void* PS_waterReflection;

	void SkinnedMeshPipeline::cascadeShadowRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void deferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void forwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	static void hook();
	RxPipeline* initGraphics();
};

extern SkinnedMeshPipeline* SkinnedMeshPipe;
