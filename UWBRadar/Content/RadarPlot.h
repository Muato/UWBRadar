#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: RadarPlot.h
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include "..\Common\D3D11class.h"
#include "..\Common\Network.h"

using namespace Platform;

struct VERTEX
{
	float X, Y, Z;
	float R, G, B, a;
};

class RadarPlot
{
public:
	RadarPlot();
	bool Initialize();
	void Run();
	void SetExampleSwapChainPanel(SwapChainPanel^ &panel);
	void Resize(Windows::Foundation::Size size);
	void Render();
	void Update(unsigned short int Data[4096]);

	void InitGraphics();
	void InitPipeline();

	Array<byte>^ LoadShaderFile(std::string File);

	std::unique_ptr <tcpip> m_tcpip;
private:
	Windows::Foundation::IAsyncAction^ m_RenderLoopWorker;

	std::unique_ptr <D3D11class> m_DXClass;
	

	bool		m_ResourcesBusy;

	// Application data
	ComPtr<ID3D11InputLayout>		m_inputLayout;
	ComPtr<ID3D11Buffer>			m_vertexBuffer;
	ComPtr<ID3D11VertexShader>		m_vertexShader;
	ComPtr<ID3D11PixelShader>		m_pixelShader;
	VERTEX Vertices[1024] = { 0.0f };

};