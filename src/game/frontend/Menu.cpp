#include "Menu.hpp"
#include "core/logging/Logging.hpp"

// External declaration for unload flag
namespace Globals {
    extern std::atomic<bool> g_bUnloadRequested;
}

namespace Menu
{
    bool g_bClientMove = false;
    float g_fNoClipSpeed = 5.0f;  // Default speed multiplier (1.0 = normal, 10.0 = very fast)
    bool g_bShowCallTraces = false;
    char g_szCallTraceFilter[1024]{};
    bool g_bCallTraceFilterSubclasses = false;
    std::string g_sCallTraceFilter{};
    std::map<size_t, CallTraceEntry_t> g_mapCallTraces{};
    
    // Feature toggles for performance
    bool g_bInstantInteract = false;
    bool g_bAutoKeypad = false;

    void CallTraceEntry_t::Draw()
    {
        if(!g_sCallTraceFilter.empty() && m_sClassName.find(g_sCallTraceFilter) == std::string::npos){
            if(!g_bCallTraceFilterSubclasses)
                return;
            
            if(std::find_if(m_vecSubClasses.begin(), m_vecSubClasses.end(), [&](const std::string& str) {
                return str.find(g_sCallTraceFilter) != std::string::npos;
            }) == m_vecSubClasses.end())
                return;
        }

        if(ImGui::CollapsingHeader(m_sClassName.c_str()))
        {
            ImGui::PushID(m_sClassName.c_str());

            if(m_vecSubClasses.size()){
                if(ImGui::TreeNode("Sub Classes"))
                {
                    for(const auto& str : m_vecSubClasses)
                        ImGui::Text("%s", str.c_str());
                    ImGui::TreePop();
                }
            }
            
            if(ImGui::TreeNode("Called Functions"))
            {
                for(const auto& pairEntry : m_mapCalledFunctions)
                    ImGui::Text("%s", pairEntry.second.c_str());
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    void PreDraw()
    {
        // Only run ESP if enabled - PERFORMANCE CRITICAL
        if (!ESP::GetConfig().bESP)
            return;
            
        SDK::UWorld* pGWorld = SDK::UWorld::GetWorld();
        if (!pGWorld)
            return;

        SDK::UGameInstance* pGameInstance = pGWorld->OwningGameInstance;
        if (!pGameInstance)
            return;

        SDK::ULocalPlayer* pLocalPlayer = pGameInstance->LocalPlayers[0];
        if (!pLocalPlayer)
            return;

        SDK::APlayerController* pPlayerController = pLocalPlayer->PlayerController;
        if (!pPlayerController)
            return;

        SDK::ULevel* pPersistentLevel = pGWorld->PersistentLevel;
        if (!pPersistentLevel)
            return;

        ESP::Render(pGWorld, pPlayerController);
    }

    void DrawEnemyESPSection(const char* szType, ESP::EnemyESP& stSettings)
    {
        if (!ImGui::BeginCombo(szType, stSettings.m_sPreviewText.c_str()))
            return;

        bool changed = false;

        changed |= ImGui::Selectable("Box", &stSettings.m_bBox);
        changed |= ImGui::Selectable("Health", &stSettings.m_bHealth);
        changed |= ImGui::Selectable("Name", &stSettings.m_bName);
        changed |= ImGui::Selectable("Outline", &stSettings.m_bOutline);

        if (changed)
            stSettings.UpdatePreviewText();

        ImGui::EndCombo();
    }

    void Draw(bool& bShowMenu)
    {
        auto& espConfig = ESP::GetConfig();

        ImGui::Begin("Payday 3 Internal", &bShowMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        
        // Performance metrics
        ImGui::Text("FPS: %.1f (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();
        
        // Features section
        if (ImGui::CollapsingHeader("Features", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::Checkbox("No Clip", &g_bClientMove))
            {
                // Toggle noclip mode
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Fly through walls and objects\nWASD to move, look direction to fly up/down");
            
            // Speed slider (only show when noclip is enabled)
            if (g_bClientMove)
            {
                ImGui::Indent();
                ImGui::SliderFloat("Speed", &g_fNoClipSpeed, 1.0f, 100.0f, "%.1fx");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("1x = Slow, 10x = Fast, 50x = Very Fast, 100x = Extreme!");
                ImGui::Unindent();
            }
        }
        
        ImGui::Separator();
        
        // ESP features
        if (ImGui::CollapsingHeader("ESP", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Enable ESP", &espConfig.bESP);
            
            if (espConfig.bESP)
            {
                ImGui::Indent();
                DrawEnemyESPSection("Normal Enemies", espConfig.m_stNormalEnemies);
                DrawEnemyESPSection("Special Enemies", espConfig.m_stSpecialEnemies);
                ImGui::Unindent();
            }
        }

        ImGui::Separator();
        
        // Debug section (only in debug builds)
#ifdef _DEBUG
        if (ImGui::CollapsingHeader("Debug"))
        {
            ImGui::Checkbox("Show Call Traces", &g_bShowCallTraces);
        }
        ImGui::Separator();
#endif
        
        // Unload button
        if (ImGui::Button("Unload", ImVec2(-1, 0)))
        {
            ImGui::OpenPopup("Confirm Unload");
        }
        
        // Unload confirmation popup
        if (ImGui::BeginPopupModal("Confirm Unload", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to unload?");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                Globals::g_bUnloadRequested = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("No", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        ImGui::End();

        // Call traces window (separate, only shown when enabled)
        if (g_bShowCallTraces)
        {
            ImGui::Begin("Call Traces", &g_bShowCallTraces);
            
            if(ImGui::Button("Clear"))
                ImGui::OpenPopup("Confirm Clear");
                
            if (ImGui::BeginPopupModal("Confirm Clear", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Are you sure you want to clear all entries?");
                ImGui::Separator();

                if (ImGui::Button("Yes", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                    g_mapCallTraces.clear();
                }

                ImGui::SameLine();

                if (ImGui::Button("No", ImVec2(120, 0)))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
            
            ImGui::InputText("##Filter", g_szCallTraceFilter, sizeof(g_szCallTraceFilter));
            ImGui::SameLine();
            ImGui::Checkbox("##Filter Use Subclasses", &g_bCallTraceFilterSubclasses);

            g_sCallTraceFilter = g_szCallTraceFilter;

            ImGui::Separator();
            for (auto& pairEntry : g_mapCallTraces)
                pairEntry.second.Draw();
            
            ImGui::End();
        }
    }

    void PostDraw() {}
}