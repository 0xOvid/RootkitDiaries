// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "winuser.h"
#include <iostream>

int notUsed() {
    std::cout << "gg";
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    int MsgBoxId = MessageBox(NULL, (LPCWSTR)L"Hello from kernel", (LPCWSTR)L"TEST", MB_HELP);
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