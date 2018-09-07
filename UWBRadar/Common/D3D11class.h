#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: D3D11class.h
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <d3d11_3.h>
#include <dxgi1_4.h>
#include "DirectXHelper.h"
#include <DirectXColors.h>
#include <windows.ui.xaml.media.dxinterop.h>
#include <tchar.h>


using namespace Microsoft::WRL;
using namespace Windows::UI::Xaml::Controls;
using namespace DX;

class D3D11class
{
public:
	void Initialize();
	bool CreateDeviceResources();
	bool CreateResizableResource();
	void SetSwapChainPanel(SwapChainPanel^ panel);
	void ResizeResources(Windows::Foundation::Size size);
	void Update();
	void Present();

	// D3D Accessors.
	ID3D11Device3*				GetDevice() const				{ return m_device.Get(); }
	ID3D11DeviceContext3*		GetDevContext() const			{ return m_devContext.Get(); }
	ID3D11RenderTargetView1*	GetRenderTargetView() const		{ return m_renderTargetView.Get(); }
private:
	// Direct3D objects
	ComPtr<ID3D11Device3>			m_device;
	ComPtr<ID3D11DeviceContext3>	m_devContext;
	ComPtr<IDXGISwapChain3>			m_swapChain;

	ComPtr<ID3D11RenderTargetView1>	m_renderTargetView;

	

	// Catched reference to the XAML panel
	SwapChainPanel^			m_swapChainPanel;

	// Catched properties
	D3D_FEATURE_LEVEL			m_FeatureLevel;
	Windows::Foundation::Size	m_Size;


};