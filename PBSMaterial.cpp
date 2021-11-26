#include "PBSMaterial.h"
#include <filesystem>


RwTexDictionary* PBSMaterialMgr::materialsTXD;
std::unordered_map<std::string, PBSMaterial*> PBSMaterialMgr::materials{};

bool has_suffix(const std::string& s, const std::string& suffix)
{
    return (s.size() >= suffix.size()) &&
        std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}
PBSMaterial::PBSMaterial(const std::string& fname) : m_sName(fname)
{
    auto file = fopen(
        (std::string("materials\\") + m_sName + ".mat").c_str(), "rt");
    char specFileName[80];
    char normalFileName[80];
    if(file)
    {
        auto res = fscanf(file, "%79s\n", specFileName);
        if(res == EOF)
        {
            fclose(file);
            return;
        }
        m_tSpecRoughness = RwTexDictionaryFindNamedTexture(
            PBSMaterialMgr::materialsTXD, specFileName);

        res = fscanf(file, "%79s\n", normalFileName);

        if(res == EOF)
        {
            fclose(file);
            return;
        }
        m_tNormals = RwTexDictionaryFindNamedTexture(
            PBSMaterialMgr::materialsTXD, normalFileName);
        fclose(file);
    }
    // m_tSpecRoughness
}

void PBSMaterialMgr::LoadMaterials()
{
    materialsTXD = CFileLoader::LoadTexDictionary("materials\\materials.txd");
    std::string     path("materials/");
    std::string     ext(".mat");
    std::error_code ec;
    if(!std::filesystem::exists(path, ec))
    {
       /* g_pDebug->printMsg(
            "Materials folder not found, material rendering will be disabled.",
            0);*/
        return;
    }

    auto dir_iter = std::filesystem::recursive_directory_iterator(path, ec);
    for(auto& p : dir_iter)
    {
        if(p.path().extension() == ext)
        {
            auto materialptr = new PBSMaterial(p.path().stem().string());
            materials[materialptr->m_sName] = materialptr;
        }
    }
}

void PBSMaterialMgr::SetMaterial(const char* textureName)
{}
