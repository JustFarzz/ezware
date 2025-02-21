#include <windows.h>
#include "ezware/ezware.h"
#include "renderer/dx11_device.h"

#include "external_window/samp_window.h"
#include "external_window/window_color_picker.h"
#include "external_window/todo_window.h"
#include "icons/IconsFontAwesome6.h"

TodoWindow todoWindow;

bool WindowSAMP = false;
bool show_color_picker = false;
bool show_todo = false;

#ifndef UNICODE
#define UNICODE
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int g_WindowWidth = 0;
int g_WindowHeight = 0;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
        {
            g_WindowWidth = LOWORD(lParam);
            g_WindowHeight = HIWORD(lParam);

        }
        return 0;
        
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    g_WindowWidth = GetSystemMetrics(SM_CXSCREEN);
    g_WindowHeight = GetSystemMetrics(SM_CYSCREEN);

    WNDCLASSEXA wc = {
        sizeof(WNDCLASSEXA),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        hInstance,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        "EzWare By Farzz",
        nullptr
    };

    RegisterClassExA(&wc);
    HWND hwnd = CreateWindowA(
        wc.lpszClassName,
        "EzWare By Farzz",
        WS_POPUP, // Bisa pakai ini sih WS_OVERLAPPEDWINDOW
        0,
        0,
        g_WindowWidth,
        g_WindowHeight,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    DX11Device dx11Device;
    if (!dx11Device.Initialize(hwnd))
    {
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    EzWare ezWare;
    if (!ezWare.Initialize(hwnd, &dx11Device))
    {
        dx11Device.Cleanup();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    if (!hwnd) {
        MessageBoxA(0, "Failed to create window!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    UpdateWindow(hwnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        auto context = dx11Device.GetContext();
        auto rtv = dx11Device.GetRenderTargetView();

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        context->ClearRenderTargetView(rtv, clearColor);

        context->OMSetRenderTargets(1, &rtv, nullptr);

        ezWare.BeginFrame();

        ImGui::SetNextWindowSize(ImVec2(float(g_WindowWidth), float(g_WindowHeight)));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::PushFont(ezWare.GetMainFont());
        ImGui::Begin("LETS BEGIN OUR PROJECT", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(intptr_t)ezWare.GetBackgroundTexture(),
            ImVec2(0, 0),
            ImVec2(float(g_WindowWidth), float(g_WindowHeight))
        );

        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - ImGui::CalcTextSize("- x").x - 100, 23));

        if (ImGui::Button("_", ImVec2(50.0f, 30.0f)))
        {
            ShowWindow(hwnd, SW_MINIMIZE);
        }

        ImGui::SameLine();

        if (ImGui::Button("X", ImVec2(50.0f, 30.0f)))
        {
            exit(0);
        }

        if (ImFont* font2 = ezWare.GetDebroseeFont())
        {
            ImGui::PushFont(font2);

            const char* text = "LETS BEGIN OUR PROJECT";
            ImVec2 textSize = ImGui::CalcTextSize(text);
            ImVec2 textPos = ImVec2(
                (ImGui::GetWindowWidth() - textSize.x) * 0.5f,
                (ImGui::GetWindowHeight() - textSize.y) * 0.0f
            );

            ImVec2 screenPos = ImGui::GetCursorScreenPos();
            screenPos.x += textPos.x;
            screenPos.y += textPos.y;

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            float paddingX = 20.0f;  // Padding horizontal
            float paddingY = 10.0f;  // Padding vertikal
            float rounding = 10.0f;  // Radius rounded

            ImVec2 bgMin = ImVec2(screenPos.x - paddingX, screenPos.y - paddingY);
            ImVec2 bgMax = ImVec2(screenPos.x + textSize.x + paddingX, screenPos.y + textSize.y + paddingY);

            drawList->AddRectFilled(bgMin, bgMax, IM_COL32(13, 27, 42, 2005), rounding);

            drawList->AddText(screenPos, IM_COL32(255, 255, 255, 255), text);

            ImGui::PopFont();
        }

        ImVec2 buttonSize = ImVec2(100, 50);
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGui::SetCursorPos(ImVec2(
            windowSize.x - buttonSize.x * 15.0f,  // Posisi X (margin kanan)
            windowSize.y - buttonSize.y * 12.0f   // Posisi Y (margin bawah)
        ));

        if (ImGui::Button("SAMP", buttonSize))
        {
            WindowSAMP = true;
        }
        Window_SAMP(&WindowSAMP);

        ImVec2 buttonColPick = ImVec2(100, 50);
        ImVec2 windowColPick = ImGui::GetWindowSize();
        ImGui::SetCursorPos(ImVec2(
            windowColPick.x - buttonColPick.x * 15.0f,
            windowColPick.y - buttonColPick.y *  10.8f
        ));

        float iconWidth = ImGui::CalcTextSize(ICON_FA_PAINTBRUSH).x;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        if (ImGui::Button("##colorBtn", buttonColPick))
        {
            show_color_picker = true;
        }
        Window_Color_Picker(&show_color_picker);

        ImVec2 buttonPos = ImGui::GetItemRectMin();
        ImGui::PushFont(ezWare.GetIconFont());
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(buttonPos.x + spacing, buttonPos.y + (buttonColPick.y - ImGui::GetFontSize()) * 0.5f),
            ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Text]),
            ICON_FA_PAINTBRUSH
        );
        ImGui::PopFont();

        float textOffsetX = iconWidth + spacing * 2;
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(buttonPos.x + textOffsetX, buttonPos.y + (buttonColPick.y - ImGui::GetFontSize()) * 0.5f),
            ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Text]),
            " COLOR"
        );

        ImVec2 buttonTodo = ImVec2(60, 50);
        ImVec2 windowTodo = ImGui::GetWindowSize();
        ImGui::SetCursorPos(ImVec2(
            windowSize.x - buttonSize.x * 0.8f,
            windowSize.y - buttonSize.y * 15.7f
        ));

        ImGui::PushFont(ezWare.GetIconFont());
        if (ImGui::Button(ICON_FA_BOOK "", buttonTodo))
        {
            show_todo = !show_todo;
        }
        ImGui::PopFont();
        
        if (show_todo)
        {
            todoWindow.Show(&show_todo);
        }

        ImGui::End();
        ImGui::PopFont();

        ezWare.EndFrame();
        ezWare.Render();

        dx11Device.GetSwapChain()->Present(1, 0);
    }

    return 0;
}