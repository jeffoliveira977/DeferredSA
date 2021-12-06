#pragma once
class CPed;
#include <list>
class VisibilityPlugins
{  
public:
    static void RenderWeaponPedsNoMuzzleFlash();
    static void RenderWeaponsForPed(CPed*);
    static void AddPedWithWeapons(CPed*);
    static void DisableGunFlashAlpha(CPed*);
    static void ClearWeaponPedsList();
    static void Patch();

private:
    static std::list<CPed*> ms_weaponPeds;
};

