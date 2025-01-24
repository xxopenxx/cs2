#include <Windows.h>
#include <MinHook.h>
#include "../../Common/include/Memory.h"

CMemory g_Memory;

typedef HANDLE(WINAPI* tOpenProcess)(DWORD, BOOL, DWORD);
tOpenProcess fpOpenProcess = nullptr;

HANDLE WINAPI DetourOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
    if (dwProcessId == g_Memory.pProcessId && dwDesiredAccess == PROCESS_ALL_ACCESS) {
        return nullptr;
    }
    return fpOpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}

void InitializeBypass() {
    g_Memory.Initialize("cs2.exe");
    
    if (MH_Initialize() == MH_OK) {
        MH_CreateHook(&OpenProcess, &DetourOpenProcess, reinterpret_cast<LPVOID*>(&fpOpenProcess));
        MH_EnableHook(MH_ALL_HOOKS);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    FreeConsole();
    InitializeBypass();
    
    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) break;
        Sleep(100);
    }
    
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    return 0;
}