#pragma once
#include "CDrawable.h"

class VehicleMeshPipeline : virtual public CDrawable
{
	void* reflectionParaboloidVS;
	void* reflectionSphericalVS;

	void* deferredPS;
	void* deferredVS;

	void reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void deferredRendering(RwResEntry* entry, void* object, RwUInt32 flags);
	void forwardRendering(RwResEntry* entry, void* object, RwUInt32 flags);
public:
	RxPipeline* initGraphics();
	static void hook();
};

extern VehicleMeshPipeline* VehicleMeshPipe;