#pragma once
#include "CDrawable.h"
#include "VertexShader.h"
#include "PixelShader.h"
class DefaultMeshPipeline : virtual public MeshRenderingMode
{
	VertexShader* reflectionParaboloidVS;
	VertexShader* reflectionSphericalVS;

	PixelShader* PS_deferred;
	VertexShader* VS_deferred;

	VertexShader* VS_simple;
	PixelShader* PS_simple;

	void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	static void Hook();
	RwBool Initialize();
};

extern DefaultMeshPipeline* DefaultMeshPipe;