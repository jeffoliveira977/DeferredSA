#pragma once
#include "CommonD.h"

class ShadowCaster
{
	std::vector<CEntity*> m_castEntity[4];
public:
	ShadowCaster();
	~ShadowCaster();

	void AddShadowEntity(int i, CEntity* entity, float distace);
	void CastShadowSectorList(CPtrList& ptrList);
	void ScanSectorList(int x, int y);
	void AddObject(CEntity* entity);
	void ClearCullList();
	void Update(int x, int y);
	void Render(int i);
	std::unordered_map<CEntity*, bool> mCulled[4];
};

extern ShadowCaster *ShadowCasterEntity;
