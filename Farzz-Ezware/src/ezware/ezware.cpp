#include "ezware.h"

#include "load_image/load_image.h"
#include "../icons/IconsFontAwesome5.h"
#include "../icons/IconsFontAwesome6.h"
#include "../icons/myIcons.h"
#include "../external_window/monitoring/cpu_process.h"
#include "../external_window/samp_window.h"

EzWare::EzWare() {}
EzWare::~EzWare() { Cleanup(); }

bool EzWare::Initialize(HWND hwnd, DX11Device* device)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();


    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //=========================================[ ICON FONT ]=========================================//

    float baseFontSize =28.0f;
    float iconFontSize = baseFontSize * 2.0f / 3.0f;

    //==========[FONT CONFIG]=============//

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

    ImFontConfig iconFont_cfg;
    iconFont_cfg.PixelSnapH = true;
    iconFont_cfg.GlyphMinAdvanceX = iconFontSize;
    //icon3_cfg.MergeMode = true;

    //====================================//

    //==========[FONT LOAD]==============//

    m_iconFont = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS_6, iconFontSize, &iconFont_cfg, icons_ranges); // Icon 3: Active

    //====================================//

    //===================================================================================================//

    //=========================================[ UNIVERSAL FONT ]=========================================//
    io.Fonts->AddFontDefault();
    
    // main font
    m_mainFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 18.5f, NULL, io.Fonts->GetGlyphRangesJapanese());
    m_mainFont2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    // font 1
    m_fFont1 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Impact.ttf", 35.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    io.Fonts->Build();

    //===================================================================================================//

    m_imageWidth = 0;
    m_imageHeight = 0;
    m_texture = NULL;

    m_bgWidth = 0;
    m_bgHeight = 0;
    m_backgroundTexture = NULL;

    ID3D11ShaderResourceView* my_texture = NULL;
    bool ret = LoadTextureFromFile(device, "D:\\SAMP_REALITY\\LOGO\\Farzz.png", &m_texture, &m_imageWidth, &m_imageHeight);
    if (!ret) {
        MessageBoxA(0, "Failed to load texture!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }
    IM_ASSERT(ret);

    bool bgRet = LoadTextureFromFile(device, "D:\\SAMP_REALITY\\LOGO\\background1.jpg", &m_backgroundTexture, &m_bgWidth, &m_bgHeight);
    if (!bgRet) {
        MessageBoxA(0, "Failed to load background texture!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ImGuiStyle& style = ImGui::GetStyle();
    
    //=========================================[ Style Warna]=========================================//
    
    // TitleBg *Not Used*
    //style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.8f, 0.5f, 0.8f, 1.0f);

    // Text Color
    style.Colors[ImGuiCol_Text]             = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Button Color
    style.Colors[ImGuiCol_Button]           = ImVec4(0.1f, 0.1f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.1f, 0.1f, 0.16f, 1.0f);

    //===============================================================================================//


    //=========================================[ Style UI]=========================================//
    
    style.WindowRounding = 10.0f;
    style.WindowBorderSize = 0.5f;
    style.FrameRounding = 5.f;
    style.GrabRounding = 3.f;
    style.ChildRounding = 3.f;
    style.FrameBorderSize = 0.0f;

    //===============================================================================================//

    if (!ImGui_ImplWin32_Init(hwnd))
    {
        MessageBoxA(0, "Failed to initialize ImGui Win32!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (!ImGui_ImplDX11_Init(device->GetDevice(), device->GetContext()))
    {
        MessageBoxA(0, "Failed to initialize ImGui DX11!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void EzWare::Cleanup()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void EzWare::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void EzWare::EndFrame()
{
    ImGui::Render();
}

void EzWare::Render()
{
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}