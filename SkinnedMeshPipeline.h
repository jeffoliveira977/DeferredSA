#pragma once
#include "CDrawable.h"
#include "VertexShader.h"
#include "PixelShader.h"

class SkinnedMeshPipeline : public MeshRenderingMode
{
protected:
	VertexShader* reflectionParaboloidVS;
	VertexShader* reflectionSphericalVS;

	PixelShader* PS_deferred;
	VertexShader* VS_deferred;

	VertexShader* VS_waterReflection;
	PixelShader* PS_waterReflection;

	void ShadowRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	static void Hook();
	RxPipeline* Initialize();
};

extern SkinnedMeshPipeline* SkinnedMeshPipe;
