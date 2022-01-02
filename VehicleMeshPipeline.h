#pragma once
#include "CDrawable.h"
#include "VertexShader.h"
#include "PixelShader.h"

class VehicleMeshPipeline : virtual public MeshRenderingMode
{
	VertexShader* reflectionParaboloidVS;
	VertexShader* reflectionSphericalVS;

	PixelShader* deferredPS;
	VertexShader* deferredVS;

	void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	RxPipeline* Initialize();
	static void Hook();
};

extern VehicleMeshPipeline* VehicleMeshPipe;