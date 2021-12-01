#pragma once
#include "CommonD.h"

class RenderableReflectionObjects
{
protected:
	virtual  void SectorList(CPtrList& ptrList);
	 void ScanSectorList(int sectorX, int sectorY);
public:
	 void Update();
};

