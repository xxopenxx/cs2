#include "../include/Memory.h"

// Definicja konstruktora
CMemory::CMemory() = default;

bool CMemory::ReadRaw(uintptr_t address, void* buffer, size_t size) {
    SIZE_T bytesRead;
    return ReadProcessMemory(pProcessHandle, (LPCVOID)address, buffer, size, &bytesRead);
}

void CMemory::Initialize(const char* processName) {
    PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    while (Process32Next(snapshot, &entry)) {
        if (!_stricmp(entry.szExeFile, processName)) {
            pProcessId = entry.th32ProcessID;
            pProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pProcessId);
            break;
        }
    }
    CloseHandle(snapshot);
}

// Definicja globalnej instancji
CMemory g_Memory;