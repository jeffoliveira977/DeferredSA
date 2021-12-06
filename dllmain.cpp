#include <Windows.h>
#include "DeferredSA.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            
            dllModule = hinstDLL;
            Hook();
            break;

        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
