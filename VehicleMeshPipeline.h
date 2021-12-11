#pragma once
#include "CDrawable.h"

class VehicleMeshPipeline : virtual public MeshRenderingMode
{
	void* reflectionParaboloidVS;
	void* reflectionSphericalVS;

	void* deferredPS;
	void* deferredVS;

	void ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	RxPipeline* initGraphics();
	static void Hook();
};

extern VehicleMeshPipeline* VehicleMeshPipe;