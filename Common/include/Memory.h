#pragma once
#include <Windows.h>
#include <TlHelp32.h>

class CMemory {
public:
    HANDLE pProcessHandle = nullptr;
    DWORD pProcessId = 0;

    CMemory();  // Deklaracja konstruktora
    void Initialize(const char* processName);
    
    template<typename T>
    T Read(uintptr_t address) {
        T buffer;
        ReadProcessMemory(pProcessHandle, (LPCVOID)address, &buffer, sizeof(T), nullptr);
        return buffer;
    }

    bool ReadRaw(uintptr_t address, void* buffer, size_t size);
};

// Deklaracja globalnej instancji
extern CMemory g_Memory;