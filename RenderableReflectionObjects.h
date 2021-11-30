#pragma once
#include "CommonD.h"

class RenderableReflectionObjects
{
	static std::vector<CEntity*> m_renderableList;

	static void AddObject(CEntity* entity, float distance); 
	static void SectorList(CPtrList& ptrList);
	static void ScanSectorList(int sectorX, int sectorY);
public:
	static void Update();
	static std::vector<CEntity*> GetRenderList();
};

