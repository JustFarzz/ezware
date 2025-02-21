
//=====================================[ This Project Require DirectX 11 ]=====================================//

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class DX11Device
{
public:
	DX11Device();
	~DX11Device();

	bool Initialize(HWND hwnd);
	void Cleanup();

	ID3D11Device* GetDevice()
	{
		return m_device.Get();
	}

	ID3D11DeviceContext* GetContext() 
	{
		return m_context.Get();
	}
	
	IDXGISwapChain* GetSwapChain()
	{ 
		return m_swapChain.Get();
	}
	
	ID3D11RenderTargetView* GetRenderTargetView()
	{ 
		return m_renderTargetView.Get(); 
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
};