#pragma once

#include "../renderer/dx11_device.h"
#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_impl_win32.h"
#include "../../include/imgui/imgui_impl_dx11.h"

class EzWare
{
public:
	EzWare();
	~EzWare();

    int g_WindowWidth = 0;
    int g_WindowHeight = 0;

    ID3D11ShaderResourceView* GetTexture() const { return m_texture; }
    ID3D11ShaderResourceView* m_backgroundTexture;

    //int GetImageWidth() const { return m_imageWidth; }
    //int GetImageHeight() const { return m_imageHeight; }

    int m_bgWidth;
    int m_bgHeight;

    ID3D11ShaderResourceView* GetBackgroundTexture() const { return m_backgroundTexture; }
    int GetBgWidth() const { return m_bgWidth; }
    int GetBgHeight() const { return m_bgHeight; }

    ImFont* GetIconFont() const { return m_iconFont; }

    ImFont* GetMainFont() const { return m_mainFont;  }
    ImFont* GetMainFont2() const { return m_mainFont2; }
    ImFont* GetDebroseeFont() const { return m_fFont1;  }

    bool Initialize(HWND hwnd, DX11Device* device);
    void Cleanup();
    void BeginFrame();
    void EndFrame();
    void Render();

private:
    ImFont* m_iconFont;

    ImFont* m_mainFont;
    ImFont* m_mainFont2;
    ImFont* m_fFont1;

    ID3D11ShaderResourceView* m_texture;
    int m_imageWidth;
    int m_imageHeight;
};