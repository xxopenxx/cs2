#pragma once
#include <cstdint>

namespace CS2SDK {
    struct Vector3 {
        float x, y, z;
    };

    class CBaseEntity {
    public:
        int32_t m_iHealth;
        int32_t m_iTeamNum;
        Vector3 m_vecOrigin;
    };

    class CEntityList; // Forward declaration
}