#include "pch.h"
#include "Drawings.hpp"
#include "imgui.h"

void drawings::DrawBox(int X, int Y, int W, int H, const ImU32& color, int thickness)
{
    // Convert once
    const float x = static_cast<float>(X);
    const float y = static_cast<float>(Y);
    const float w = static_cast<float>(W);
    const float h = static_cast<float>(H);

    const float lineW = w;
    const float lineH = h;

    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    const ImU32 black = ImGui::ColorConvertFloat4ToU32(
        ImVec4(1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f, 1.0f)
    );

    // Black outline
    drawList->AddLine(ImVec2(x, y), ImVec2(x, y + lineH), black, 3.0f);
    drawList->AddLine(ImVec2(x, y), ImVec2(x + lineW, y), black, 3.0f);
    drawList->AddLine(ImVec2(x + w - lineW, y), ImVec2(x + w, y), black, 3.0f);
    drawList->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + lineH), black, 3.0f);

    drawList->AddLine(ImVec2(x, y + h - lineH), ImVec2(x, y + h), black, 3.0f);
    drawList->AddLine(ImVec2(x, y + h), ImVec2(x + lineW, y + h), black, 3.0f);
    drawList->AddLine(ImVec2(x + w - lineW, y + h), ImVec2(x + w, y + h), black, 3.0f);
    drawList->AddLine(ImVec2(x + w, y + h - lineH), ImVec2(x + w, y + h), black, 3.0f);

    // Colored corners
    drawList->AddLine(ImVec2(x, y), ImVec2(x, y + lineH), color, static_cast<float>(thickness));
    drawList->AddLine(ImVec2(x, y), ImVec2(x + lineW, y), color, static_cast<float>(thickness));
    drawList->AddLine(ImVec2(x + w - lineW, y), ImVec2(x + w, y), color, static_cast<float>(thickness));
    drawList->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + lineH), color, static_cast<float>(thickness));

    drawList->AddLine(ImVec2(x, y + h - lineH), ImVec2(x, y + h), color, static_cast<float>(thickness));
    drawList->AddLine(ImVec2(x, y + h), ImVec2(x + lineW, y + h), color, static_cast<float>(thickness));
    drawList->AddLine(ImVec2(x + w - lineW, y + h), ImVec2(x + w, y + h), color, static_cast<float>(thickness));
    drawList->AddLine(ImVec2(x + w, y + h - lineH), ImVec2(x + w, y + h), color, static_cast<float>(thickness));
}