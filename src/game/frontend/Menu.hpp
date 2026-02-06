#pragma once

#include <map>
#include <string>
#include <vector>
#include <imgui.h>
#include "types/gameoffsets/SDK.hpp"
#include "types/config/Config.hpp"
#include "game/features/esp/ESP.hpp"

namespace Menu
{
    extern bool g_bClientMove;
    extern float g_fNoClipSpeed;  // Speed multiplier for noclip
    extern bool g_bShowCallTraces;
    extern char g_szCallTraceFilter[1024];
    extern bool g_bCallTraceFilterSubclasses;
    extern std::string g_sCallTraceFilter;

    struct CallTraceEntry_t {
        std::string m_sClassName{};
        std::vector<std::string> m_vecSubClasses{};
        std::map<size_t, std::string> m_mapCalledFunctions{};

        void Draw();
    };

    extern std::map<size_t, CallTraceEntry_t> g_mapCallTraces;

    void PreDraw();
    void Draw(bool& bShowMenu);
    void PostDraw();
}