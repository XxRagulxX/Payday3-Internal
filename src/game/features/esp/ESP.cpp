#include "ESP.hpp"
#include "types/gameoffsets/SDK.hpp"

#include <imgui.h>
#include <unordered_map>
#include <string>
#include <cmath>

enum class EActorType
{
    Other,
    Guard,
    Shield,
    Dozer,
    Cloaker,
    Civilian
};

struct ClassCacheEntry
{
    EActorType Type;
};

static std::unordered_map<SDK::UClass*, ClassCacheEntry> g_ClassCache;

static EActorType GetActorTypeCached(SDK::AActor* actor)
{
    if (!actor || !actor->Class)
        return EActorType::Other;

    auto cls = actor->Class;

    auto it = g_ClassCache.find(cls);
    if (it != g_ClassCache.end())
        return it->second.Type;

    std::string name = cls->Name.ToString();
    EActorType type = EActorType::Other;

    if (name.find("Civilian") != std::string::npos)
        type = EActorType::Civilian;
    else if (name.find("Dozer") != std::string::npos)
        type = EActorType::Dozer;
    else if (name.find("Shield") != std::string::npos)
        type = EActorType::Shield;
    else if (name.find("Cloaker") != std::string::npos)
        type = EActorType::Cloaker;
    else if (actor->IsA(SDK::ASBZAICharacter::StaticClass()))
        type = EActorType::Guard;

    g_ClassCache.emplace(cls, ClassCacheEntry{ type });
    return type;
}

static bool GetScreenBox(
    SDK::APlayerController* pc,
    SDK::AActor* actor,
    ImVec4& outBox
)
{
    SDK::FVector origin{}, extent{};
    actor->GetActorBounds(true, &origin, &extent, false);

    SDK::FVector top = origin + SDK::FVector(0, 0, extent.Z);
    SDK::FVector bottom = origin - SDK::FVector(0, 0, extent.Z);

    SDK::FVector2D sTop{}, sBottom{};
    if (!pc->ProjectWorldLocationToScreen(top, &sTop, false) ||
        !pc->ProjectWorldLocationToScreen(bottom, &sBottom, false))
        return false;

    float height = std::fabs(sBottom.Y - sTop.Y);
    float width = height * 0.35f;

    outBox = ImVec4(
        sTop.X - width,
        sTop.Y,
        sTop.X + width,
        sBottom.Y
    );
    return true;
}

static void DrawHealthBar(
    ImDrawList* draw,
    const ImVec4& box,
    float healthPct
)
{
    if (healthPct < 0.f) healthPct = 0.f;
    if (healthPct > 1.f) healthPct = 1.f;

    float h = box.w - box.y;
    float x = box.x - 6.f;

    draw->AddRectFilled(
        { x - 1, box.y - 1 },
        { x + 4, box.w + 1 },
        IM_COL32(20, 20, 20, 200)
    );

    draw->AddRectFilled(
        { x, box.y + h * (1.f - healthPct) },
        { x + 3, box.w },
        IM_COL32(0, 255, 0, 220)
    );
}

static void DrawEnemyESP(
    ImDrawList* draw,
    SDK::APlayerController* pc,
    SDK::ASBZAICharacter* enemy,
    ESP::EnemyESP& cfg
)
{
    if (!enemy || !enemy->bIsAlive)
        return;

    ImVec4 box{};
    if (!GetScreenBox(pc, enemy, box))
        return;

    if (cfg.m_bBox)
    {
        draw->AddRect(
            { box.x, box.y },
            { box.z, box.w },
            IM_COL32(255, 0, 0, 200),
            0.f, 0, 1.5f
        );
    }

    if (cfg.m_bHealth && enemy->AttributeSet)
    {
        float hp =
            enemy->AttributeSet->Health.CurrentValue /
            enemy->AttributeSet->HealthMax.CurrentValue;

        DrawHealthBar(draw, box, hp);
    }

    if (cfg.m_bName)
    {
        draw->AddText(
            { box.x, box.y - 14.f },
            IM_COL32(255, 255, 255, 220),
            "Enemy"
        );
    }
}

namespace ESP
{
    Config& GetConfig()
    {
        static Config cfg{};
        return cfg;
    }

    void Render(SDK::UWorld* world, SDK::APlayerController* pc)
    {
        if (!GetConfig().bESP || !world || !pc)
            return;

        auto* runtime =
            reinterpret_cast<SDK::USBZWorldRuntime*>(
                SDK::USBZWorldRuntime::GetWorldRuntime(world));

        if (!runtime || !runtime->AllAlivePawns)
            return;

        ImDrawList* draw = ImGui::GetBackgroundDrawList();

        auto& pawns = runtime->AllAlivePawns->Objects;

        for (int i = 0; i < pawns.Num(); ++i)
        {
            if (!pawns.IsValidIndex(i))
                continue;

            auto actor = reinterpret_cast<SDK::AActor*>(pawns[i]);
            if (!actor)
                continue;

            EActorType type = GetActorTypeCached(actor);

            if (type == EActorType::Guard ||
                type == EActorType::Shield ||
                type == EActorType::Dozer ||
                type == EActorType::Cloaker)
            {
                DrawEnemyESP(
                    draw,
                    pc,
                    reinterpret_cast<SDK::ASBZAICharacter*>(actor),
                    GetConfig().m_stNormalEnemies
                );
            }
        }
    }
}