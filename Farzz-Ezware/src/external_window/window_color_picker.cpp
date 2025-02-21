#include "window_color_picker.h"
#include <stdio.h>

#include "../icons/IconsFontAwesome6.h"

void Window_Color_Picker(bool* show)
{
	static ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	static char hex_color[10] = "#FFFFFFFF";

	if (!*show)
		return;

    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1f, 0.1f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.1f, 0.1f, 0.16f, 1.0f));

	ImGui::SetNextWindowSize(ImVec2(700, 450));
	ImGui::Begin("Color Picker", show,
		ImGuiWindowFlags_NoResize);

    ImGui::PushItemWidth(300);
;    if (ImGui::ColorPicker4("Color##picker", (float*)&color,
        ImGuiColorEditFlags_AlphaBar |
        ImGuiColorEditFlags_DisplayRGB |
        ImGuiColorEditFlags_DisplayHex))
    {
        // Update hex color string when color changes
        sprintf_s(hex_color, "#%02X%02X%02X%02X",
            (int)(color.x * 255.0f),
            (int)(color.y * 255.0f),
            (int)(color.z * 255.0f),
            (int)(color.w * 255.0f));
    }
    ImGui::PopItemWidth();

    
//=========================================[ COLOR FORMAT ]=========================================//

    // Color RGBA
    char text[100];
    sprintf_s(text, "R: %.3f G: %.3f B: %.3f A: %.3f", color.x, color.y, color.z, color.w);

    // Color RGBA
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImVec2 textPos = ImVec2(
        (ImGui::GetWindowWidth() - textSize.x) * 0.8f,
        (ImGui::GetWindowHeight() - textSize.y) * -0.5f
    );

    // Color RGBA
    ImVec2 screenPos = ImGui::GetCursorScreenPos();
    screenPos.x += textPos.x;
    screenPos.y += textPos.y;
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float paddingX = 20.0f;  // Padding horizontal
    float paddingY = 10.0f;  // Padding vertikal
    float rounding = 10.0f;  // Radius sudut bulat

    // Color RGBA
    ImVec2 bgMin = ImVec2(screenPos.x - paddingX, screenPos.y - paddingY);
    ImVec2 bgMax = ImVec2(screenPos.x + textSize.x + paddingX, screenPos.y + textSize.y + paddingY);

    // Color RGBA
    drawList->AddRectFilled(bgMin, bgMax, IM_COL32(185, 110, 11, 222), rounding);

    // Color RGBA
    drawList->AddText(screenPos, IM_COL32(255, 255, 255, 255), text);

//=======================================================================================================//

    ImGui::End();
    ImGui::PopStyleColor(3);
}