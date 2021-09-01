
#include "pch.h"
#include "Capturer.h"

#include "d32/detours.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment (lib,"d32/detours.lib")

#include <string>
using namespace std;



#define HR(x) if(!x)return NULL;

HMODULE hCodeModule = NULL;
extern HMODULE SelfHandle;
extern HMODULE LastLoaded;

void InjectDLL(HANDLE hProcess, LPCWSTR dllFilePathName)
{
	int cch = 1 + lstrlenW(dllFilePathName);
	int cb = cch * sizeof(wchar_t);
	LPWSTR PszLibFileRemote = (LPWSTR)VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, PszLibFileRemote, (LPVOID)dllFilePathName, cb, NULL);
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW"), PszLibFileRemote, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
}

extern "C" __declspec(dllexport) void Clean()
{
	HMODULE hDataDll = GetModuleHandle(_T("Data.dll"));
	FreeLibrary(hDataDll);
	FreeLibraryAndExitThread(SelfHandle, 0);
}

extern "C" __declspec(dllexport) void UnHook(LPCSTR lpCommand)//在被hook的进程中执行
{
	string cmd = lpCommand;

	string lpDllName = "";
	string lpFunName = "";
	string lpFakeName = "";
	string lpPfnAddr = "";

	int mode = 0;
	for (int i = 0; i < cmd.length(); i++) {
		if (cmd[i] == '?') {
			mode++;
			continue;
		}

		if (mode == 0) {
			lpDllName += cmd[i];
		}
		if (mode == 1) {
			lpFunName += cmd[i];
		}
		if (mode == 2) {
			lpFakeName += cmd[i];
		}
		if (mode == 3) {
			lpPfnAddr += cmd[i];
		}
	}
	

	HMODULE hData = GetModuleHandle(_T("Data.dll"));

	LPVOID* PfnOld = (LPVOID*)GetProcAddress(hData, lpPfnAddr.c_str());


	FARPROC NEW = GetProcAddress(hData, lpFakeName.c_str());

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach((void**)&(*PfnOld), NEW);//撤销拦截函数
	DetourTransactionCommit();//
}
extern "C" __declspec(dllexport) void Hook(LPCSTR lpCommand)//在被hook的进程中执行
{
	string cmd = lpCommand;
	
	string lpDllName = "";
	string lpFunName = "";
	string lpFakeName = "";
	string lpPfnAddr = "";

	int mode = 0;
	for (int i = 0; i < cmd.length(); i++) {
		if (cmd[i] == '?') {
			mode++;
			continue;
		}

		if (mode == 0) {
			lpDllName += cmd[i];
		}
		if (mode == 1) {
			lpFunName += cmd[i];
		}
		if (mode == 2) {
			lpFakeName += cmd[i];
		}
		if (mode == 3) {
			lpPfnAddr += cmd[i];
		}
	}

	HMODULE hData = GetModuleHandle(_T("Data.dll"));

	FARPROC OLD = GetProcAddress(GetModuleHandleA(lpDllName.c_str()), lpFunName.c_str());
	FARPROC NEW = GetProcAddress(hData, lpFakeName.c_str());

	LPVOID* PfnOld= (LPVOID*)GetProcAddress(hData, lpPfnAddr.c_str());
	

	DetourRestoreAfterWith();//恢复原来状态
	DetourTransactionBegin();//拦截开始
	DetourUpdateThread(GetCurrentThread());//刷新当前线程
	DetourAttach((void**)&OLD, NEW);
	DetourTransactionCommit();//拦截生效

	*PfnOld = OLD;
}
extern "C" __declspec(dllexport) BOOL CapturerPrepare() //在要hook别人进程的进程中执行
{
	hCodeModule = LoadLibrary(_T("Data.dll"));

	if (hCodeModule)return TRUE;
	else return FALSE;
}

extern "C" __declspec(dllexport) LPCapturerData CapturerStart(DWORD Pid)
{
	LPCapturerData r = (LPCapturerData)malloc(sizeof(CapturerData));//为返回的数据分配内存
	HR(r);


	r->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);//打开进程

	HR(r->hProcess);

	WCHAR Path[MAX_PATH];
	GetModuleFileNameW(SelfHandle, Path, MAX_PATH);//获取自身全路径

	InjectDLL(r->hProcess, Path);
	r->hDll = LastLoaded;//获取自身在远程进程中的基址

	HR(r->hDll);

	GetModuleFileNameW(hCodeModule, Path, MAX_PATH);
	InjectDLL(r->hProcess, Path);

	return r;
}
extern "C" __declspec(dllexport) void CapturerCatch(LPCapturerData data, LPCSTR lpDllName, LPCSTR lpFunName, LPCSTR lpFakeName, LPCSTR lpPfnAddr)//在要hook别人进程的进程中执行
{

	

	string cmd = "";
	cmd += lpDllName;
	cmd += "?";
	cmd += lpFunName;
	cmd += "?";
	cmd += lpFakeName;
	cmd += "?";
	cmd += lpPfnAddr;

	LPSTR RemoteHook = (LPSTR)data->hDll;
	RemoteHook += (unsigned long long)Hook - (unsigned long long)SelfHandle;

	int cch = 1 + lstrlenA(cmd.c_str());
	int cb = cch * sizeof(char);
	LPSTR PszLibFileRemote = (LPSTR)VirtualAllocEx(data->hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(data->hProcess, PszLibFileRemote, (LPCVOID)cmd.c_str(), cb, NULL);
	HANDLE hThread = CreateRemoteThread(data->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)RemoteHook, PszLibFileRemote, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);


}

extern "C" __declspec(dllexport) void CapturerRelease(LPCapturerData data, LPCSTR lpDllName, LPCSTR lpFunName, LPCSTR lpFakeName, LPCSTR lpPfnAddr)//在要hook别人进程的进程中执行
{

	string cmd = "";
	cmd += lpDllName;
	cmd += "?";
	cmd += lpFunName;
	cmd += "?";
	cmd += lpFakeName;
	cmd += "?";
	cmd += lpPfnAddr;

	LPSTR RemoteUnHook = (LPSTR)data->hDll;
	RemoteUnHook += (unsigned long long)UnHook - (unsigned long long)SelfHandle;

	int cch = 1 + lstrlenA(cmd.c_str());
	int cb = cch * sizeof(char);
	LPSTR PszLibFileRemote = (LPSTR)VirtualAllocEx(data->hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(data->hProcess, PszLibFileRemote, (LPCVOID)cmd.c_str(), cb, NULL);
	HANDLE hThread = CreateRemoteThread(data->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)RemoteUnHook, PszLibFileRemote, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	
}

extern "C" __declspec(dllexport) void CapturerClean(LPCapturerData data) 
{
	LPSTR RemoteClean = (LPSTR)data->hDll;
	RemoteClean += (unsigned long long)Clean - (unsigned long long)SelfHandle;

	
	HANDLE hThread = CreateRemoteThread(data->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)Clean, NULL, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);


}