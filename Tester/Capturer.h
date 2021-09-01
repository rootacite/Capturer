#pragma once


struct CapturerData {
	HANDLE hProcess;
	HMODULE hDll;
};

typedef CapturerData* LPCapturerData;

extern "C" __declspec(dllexport) BOOL CapturerPrepare(); //Load the dll where the fun is,name must be "Data.dll"
extern "C" __declspec(dllexport) LPCapturerData CapturerStart(DWORD Pid);

extern "C" __declspec(dllexport) void CapturerCatch(
	LPCapturerData data,//The id of the specified process
	LPCSTR lpDllName, //Specifie where the function specified in next param is
	LPCSTR lpFunName, //The function you want to catch
	LPCSTR lpFakeName, //The function to replace the function specified in last param
	LPCSTR lpPfnAddr //Specifie where to save old addr
);

extern "C" __declspec(dllexport) void CapturerRelease(
	LPCapturerData data,
	LPCSTR lpDllName, //Specifie where the function specified in next param is
	LPCSTR lpFunName, //The function you want to catch
	LPCSTR lpFakeName, //The function to replace the function specified in last param
	LPCSTR lpPfnAddr //Specifie where to save old addr
);

extern "C" __declspec(dllexport) void CapturerClean(LPCapturerData data);