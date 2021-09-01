// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"



extern "C" __declspec(dllexport)
FARPROC pfnAddr = NULL;


extern "C" __declspec(dllexport)

int
WINAPI
FakeMsgBox(
     HWND hWnd,
     LPCSTR lpText,
     LPCSTR lpCaption,
     UINT uType) {
    return ((int(WINAPI* )(HWND, LPCSTR, LPCSTR, UINT))pfnAddr)(0, "sdadsadadsa", "", 0);
  
}

extern "C" __declspec(dllexport)
FARPROC pfnAddrW = NULL;


extern "C" __declspec(dllexport)

int
WINAPI
FakeMsgBoxW(
    HWND hWnd,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType) {
    return ((int(WINAPI*)(HWND, LPCWSTR, LPCWSTR, UINT))pfnAddrW)(0, L"sdadsadadsa", L"", 0);

}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

