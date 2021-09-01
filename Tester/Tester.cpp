// Tester.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <atlconv.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "Capturer.h"
#pragma comment(lib,"Capturer.lib")

#include <string>
using namespace std;

//根据进程名获取PID
DWORD GetProcessIdByProcessName(const char* pszProcessName)
{
    //1.创建进程快照
    HANDLE hSnap = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,            //遍历进程快照1
        0);                            //进程PID
    if (INVALID_HANDLE_VALUE == hSnap)
    {
      //  MessageBox("创建进程快照失败！");
        return 0;
    }

    //2.获取第一条进程快照信息
    PROCESSENTRY32  stcPe = { sizeof(stcPe) };
    if (Process32First(hSnap, &stcPe))
    {

        //3.循环遍历进程Next
        do {

            //获取快照信息
            USES_CONVERSION;
            wstring ProcessName = A2T(pszProcessName);
            if (!lstrcmp(stcPe.szExeFile, ProcessName.c_str()))
            {
                //4.关闭句柄
                CloseHandle(hSnap);
                return stcPe.th32ProcessID;
            }

        } while (Process32Next(hSnap, &stcPe));

    }

    //4.关闭句柄
    CloseHandle(hSnap);
    return 0;
}

int main()
{
    CapturerPrepare();
    auto r = CapturerStart(GetProcessIdByProcessName("ApiCaller.exe"));
    CapturerCatch(r, "User32.dll", "MessageBoxA", "FakeMsgBox", "pfnAddr");

    getchar();
    CapturerCatch(r, "User32.dll", "MessageBoxW", "FakeMsgBoxW", "pfnAddrW");

    getchar();

    CapturerRelease(r, "User32.dll", "MessageBoxA", "FakeMsgBox", "pfnAddr");
    getchar();

    CapturerRelease(r, "User32.dll", "MessageBoxW", "FakeMsgBoxW", "pfnAddrW");

    getchar();

    CapturerClean(r);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
