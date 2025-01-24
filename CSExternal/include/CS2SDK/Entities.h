#pragma once
#include "SDK.h"
#include "../../../Common/include/offsets.hpp"  // Poprawiona ścieżka

namespace CS2SDK {
    class CEntityList {
    public:
        static uintptr_t GetEntity(uint32_t index) {
            return g_Memory.Read<uintptr_t>(offsets::dwEntityList + index * 0x8);
        }
    };
}