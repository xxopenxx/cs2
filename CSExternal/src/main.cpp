#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <MinHook.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "../../Common/include/Memory.h"
#include "../../Common/include/offsets.hpp"
#include "CS2SDK/SDK.h"
#include "CS2SDK/Entities.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Zmienne DirectX
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11RenderTargetView* pRenderTarget = nullptr;
WNDPROC oWndProc = nullptr;
HWND hWindow = nullptr;

// Hook Present
HRESULT(__stdcall* oPresent)(IDXGISwapChain*, UINT, UINT) = nullptr;

// Zmienne stanu
bool g_ShowMenu = true;
bool g_IsRunning = true;

bool WorldToScreen(const CS2SDK::Vector3& worldPos, ImVec2& screenPos) {
    float viewMatrix[16];
    if (!g_Memory.ReadRaw(offsets::dwViewMatrix, &viewMatrix, sizeof(viewMatrix)))
        return false;

    float clipCoords[4];
    clipCoords[0] = worldPos.x * viewMatrix[0] + worldPos.y * viewMatrix[4] + worldPos.z * viewMatrix[8] + viewMatrix[12];
    clipCoords[1] = worldPos.x * viewMatrix[1] + worldPos.y * viewMatrix[5] + worldPos.z * viewMatrix[9] + viewMatrix[13];
    clipCoords[2] = worldPos.x * viewMatrix[2] + worldPos.y * viewMatrix[6] + worldPos.z * viewMatrix[10] + viewMatrix[14];
    clipCoords[3] = worldPos.x * viewMatrix[3] + worldPos.y * viewMatrix[7] + worldPos.z * viewMatrix[11] + viewMatrix[15];

    if (clipCoords[3] < 0.1f)
        return false;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    screenPos.x = (clipCoords[0] / clipCoords[3] + 1.0f) * 0.5f * displaySize.x;
    screenPos.y = (1.0f - (clipCoords[1] / clipCoords[3])) * 0.5f * displaySize.y;

    return true;
}

void RenderESP() {
    if (!g_Memory.pProcessHandle) return;

    uintptr_t localPlayer = g_Memory.Read<uintptr_t>(offsets::dwLocalPlayerPawn);
    if (!localPlayer) return;

    CS2SDK::CBaseEntity localEntity;
    if (!g_Memory.ReadRaw(localPlayer, &localEntity, sizeof(CS2SDK::CBaseEntity)))
        return;

    std::vector<CS2SDK::CBaseEntity> players;

    for (int i = 0; i < 64; i++) {
        uintptr_t entityAddr = CS2SDK::CEntityList::GetEntity(i);
        if (!entityAddr) continue;

        CS2SDK::CBaseEntity entity;
        if (!g_Memory.ReadRaw(entityAddr, &entity, sizeof(CS2SDK::CBaseEntity)))
            continue;

        if (entity.m_iTeamNum == localEntity.m_iTeamNum)
            continue;

        ImVec2 screenPos;
        if (WorldToScreen(entity.m_vecOrigin, screenPos)) {
            players.push_back(entity);
        }
    }

    for (const auto& player : players) {
        ImVec2 screenPos;
        if (WorldToScreen(player.m_vecOrigin, screenPos)) {
            float dx = player.m_vecOrigin.x - localEntity.m_vecOrigin.x;
            float dy = player.m_vecOrigin.y - localEntity.m_vecOrigin.y;
            float dz = player.m_vecOrigin.z - localEntity.m_vecOrigin.z;
            float distance = std::sqrt(dx*dx + dy*dy + dz*dz) / 100.0f;

            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2(screenPos.x - 15, screenPos.y - 30),
                ImVec2(screenPos.x + 15, screenPos.y + 10),
                IM_COL32(255, 0, 0, 255)
            );

            char buffer[32];
            sprintf_s(buffer, sizeof(buffer), "%.1fm", distance);
            ImGui::GetBackgroundDrawList()->AddText(
                ImVec2(screenPos.x - 10, screenPos.y + 15),
                IM_COL32(255, 255, 255, 255),
                buffer
            );
        }
    }
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_ShowMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pChain, UINT SyncInterval, UINT Flags) {
    if (!pDevice) {
        pChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
        pDevice->GetImmediateContext(&pContext);

        DXGI_SWAP_CHAIN_DESC desc;
        pChain->GetDesc(&desc);
        hWindow = desc.OutputWindow;
        oWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hWindow);
        ImGui_ImplDX11_Init(pDevice, pContext);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_INSERT) & 1)
        g_ShowMenu = !g_ShowMenu;

    if (g_ShowMenu) {
        ImGui::Begin("CS2 External ESP");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    RenderESP();

    ImGui::Render();
    pContext->OMSetRenderTargets(1, &pRenderTarget, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pChain, SyncInterval, Flags);
}

void MainLoop() {
    g_Memory.Initialize("cs2.exe");

    // Czekaj aż gra będzie gotowa
    while (!hWindow) {
        hWindow = FindWindowA("Valve001", nullptr);
        Sleep(100);
    }

    // Inicjalizacja MinHook
    if (MH_Initialize() == MH_OK) {
        void** pVTable = *reinterpret_cast<void***>(pSwapChain);
        MH_CreateHook(pVTable[8], &hkPresent, reinterpret_cast<void**>(&oPresent));
        MH_EnableHook(MH_ALL_HOOKS);
    }

    // Główna pętla
    while (g_IsRunning) {
        if (GetAsyncKeyState(VK_END) & 0x8000)
            g_IsRunning = false;
        
        Sleep(10);
    }

    // Sprzątanie
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

int main() {
    MainLoop();
    return 0;
}