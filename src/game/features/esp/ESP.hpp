#pragma once

#include <string>
#include "types/gameoffsets/SDK.hpp"

namespace ESP
{
    struct EnemyESP
    {
        bool m_bBox    = true;
        bool m_bHealth = true;
        bool m_bName   = false;
        bool m_bOutline = false;

        // Optional UI preview string (cheap, updated only on toggle)
        std::string m_sPreviewText = "None";

        inline void UpdatePreviewText()
        {
            m_sPreviewText.clear();

            auto append = [&](const char* txt, bool enabled)
            {
                if (!enabled) return;
                if (!m_sPreviewText.empty())
                    m_sPreviewText += ", ";
                m_sPreviewText += txt;
            };

            append("Box", m_bBox);
            append("Health", m_bHealth);
            append("Name", m_bName);
            append("Outline", m_bOutline);

            if (m_sPreviewText.empty())
                m_sPreviewText = "None";
        }
    };

    struct Config
    {
        bool bESP = false;

        EnemyESP m_stNormalEnemies{};
        EnemyESP m_stSpecialEnemies{};
    };

    Config& GetConfig();

    void Render(SDK::UWorld* pGWorld, SDK::APlayerController* pPlayerController);
}