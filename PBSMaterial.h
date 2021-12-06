#pragma once
#include "game_sa/CFileLoader.h"
#include <unordered_map>

class PBSMaterial
{
public:
    PBSMaterial(const std::string& fname);
    std::string m_sName{};
    RwTexture* m_tSpecRoughness{};
    RwTexture* m_tNormals{};
};

class PBSMaterialMgr
{
public:
    static void LoadMaterials();
    static void SetMaterial(const char* textureName);
    static RwTexDictionary* materialsTXD;
    static std::unordered_map<std::string, PBSMaterial*> materials;
};
