////////////////////////////////////////////////////////////////////////////////
// Filename: D3D11class.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "D3D11class.h"

void D3D11class::Initialize()
{
	// Can check if each Init component returns true
	CreateDeviceResources();
	CreateResizableResource();

}

bool D3D11class::CreateDeviceResources()
{
	HRESULT hr;
	// TODO: Do I need to set feature levels?
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Define temporary pointers to a device and a device context
	ComPtr<ID3D11Device> dev;
	ComPtr<ID3D11DeviceContext> devCon;

	// Create Device and DeviceContext objects
	hr = D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		0,							// Set debug and Direct2D compatibility flags.
		featureLevels,				// TODO: List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&dev,						// Returns the Direct3D device created.
		&m_FeatureLevel,			// Returns feature level of device created.
		&devCon						// Returns the device immediate context.
		);

	ThrowIfFailed(hr);

	// Convert the pointers from one DirectX verstion to other
	dev.As(&m_device);
	devCon.As(&m_devContext);

	return true;
}

bool D3D11class::CreateResizableResource()
{
	HRESULT hr;
	
	// Clear the previous windows size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_devContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_renderTargetView = nullptr;
	m_devContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	
	if (m_swapChain != nullptr)
	{
		hr = m_swapChain->ResizeBuffers(
			2,
			max(m_Size.Width, 1),
			max(m_Size.Height, 1),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

		// TODO: Can check if Device removed and handle removal
		ThrowIfFailed(hr);
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 scd = { 0 };

		scd.Width = max(m_Size.Width, 1);
		scd.Height =  max(m_Size.Height, 1);
		scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scd.Stereo = false;
		scd.SampleDesc.Count = 1;							// Don't use multi sampling
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//
		scd.BufferCount = 2;								// Use double buffering
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	//
		scd.Flags = 0;
		scd.Scaling = DXGI_SCALING_STRETCH;
		scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// This sequence obtains the DXGI factory that was used to create Direct3D device before
		ComPtr<IDXGIDevice3> dxgiDevice;
		hr = m_device.As(&dxgiDevice);
		ThrowIfFailed(hr);

		ComPtr<IDXGIAdapter> dxgiAdapter;
		hr = dxgiDevice->GetAdapter(&dxgiAdapter);
		ThrowIfFailed(hr);

		ComPtr<IDXGIFactory4> dxgiFactory;
		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		ThrowIfFailed(hr);

		ComPtr<IDXGISwapChain1> swapChain;
		hr = dxgiFactory->CreateSwapChainForComposition(
			m_device.Get(),
			&scd,
			nullptr,
			&swapChain
			);
		ThrowIfFailed(hr);

		hr = swapChain.As(&m_swapChain);
		ThrowIfFailed(hr);

		ComPtr<ISwapChainPanelNative>	swapChainNative;
		IInspectable* panelInspectable = (IInspectable*) reinterpret_cast<IInspectable*>(m_swapChainPanel);
		panelInspectable->QueryInterface(__uuidof(ISwapChainPanelNative), (void **)&swapChainNative);

		swapChainNative->SetSwapChain(m_swapChain.Get());

		hr = dxgiDevice->SetMaximumFrameLatency(1);
		ThrowIfFailed(hr);

		

	}

	D3D11_VIEWPORT viewPort = { 0 };
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = max(m_Size.Width, 1);
	viewPort.Height = max(m_Size.Height, 1);

	m_devContext->RSSetViewports(1, &viewPort);

	ComPtr<ID3D11Texture2D1> backBuffer;
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	ThrowIfFailed(hr);

	hr = m_device->CreateRenderTargetView1(
		backBuffer.Get(),
		nullptr,
		&m_renderTargetView
	);
	ThrowIfFailed(hr);

	return true;
}

void D3D11class::SetSwapChainPanel(SwapChainPanel^ panel)
{
	m_swapChainPanel = panel;
	m_Size = Windows::Foundation::Size(static_cast<float>(panel->ActualWidth), static_cast<float>(panel->ActualHeight));;

}

void D3D11class::ResizeResources(Windows::Foundation::Size size)
{
	if (m_Size != size)
	{
		m_Size = size;
		CreateResizableResource();
	}
}

void D3D11class::Present()
{
	m_swapChain->Present(1, 0);
}


