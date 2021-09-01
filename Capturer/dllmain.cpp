// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

HMODULE SelfHandle = NULL;


#pragma data_seg("shareddata") // 名称可以

HMODULE LastLoaded = NULL;

#pragma data_seg()
#pragma comment(linker,"/section:shareddata,rws")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LastLoaded = hModule;
        SelfHandle = hModule;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

