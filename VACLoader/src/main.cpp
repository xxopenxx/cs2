#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcessId(const char* processName) {
    PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    while (Process32Next(snapshot, &entry)) {
        if (!_stricmp(entry.szExeFile, processName)) {
            CloseHandle(snapshot);
            return entry.th32ProcessID;
        }
    }
    CloseHandle(snapshot);
    return 0;
}

int main() {
    DWORD pid = GetProcessId("cs2.exe");
    if (pid == 0) {
        MessageBoxA(0, "CS2 not running!", "Error", MB_ICONERROR);
        return 1;
    }

    // Wstrzykiwanie VACBypass.dll
    HMODULE dll = LoadLibraryA("VACBypass.dll");
    if (!dll) {
        MessageBoxA(0, "Failed to load VACBypass!", "Error", MB_ICONERROR);
        return 1;
    }

    MessageBoxA(0, "VAC Bypass activated!", "Success", MB_OK);
    return 0;
}