////////////////////////////////////////////////////////////////////////////////
// Filename: RadarPlot.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "RadarPlot.h"

using namespace Windows::Foundation;
using namespace Windows::System::Threading;



RadarPlot::RadarPlot()
{
	m_ResourcesBusy = false;
	m_DXClass = std::unique_ptr<D3D11class>(new D3D11class);
	m_tcpip = std::unique_ptr<tcpip>(new tcpip);
}


bool RadarPlot::Initialize()
{

	m_DXClass->Initialize();
	InitGraphics();
	InitPipeline();

	return true;
}


void RadarPlot::Run()
{
	// Create new thread that will run the main loop for application
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		while (action->Status == AsyncStatus::Started)
		{
			if (!m_ResourcesBusy)
			{
				
				if(m_tcpip->DataAcq)
				{
					m_tcpip->DataAcq = false;
					Update(m_tcpip->Data);
				}
				Render();
				m_DXClass->Present();

			}



		}
	});

	m_RenderLoopWorker = ThreadPool::RunAsync(workItemHandler);
}


void RadarPlot::SetExampleSwapChainPanel(SwapChainPanel^ &panel)
{
	m_DXClass->SetSwapChainPanel(panel);
}


void RadarPlot::Resize(Windows::Foundation::Size size)
{
	m_ResourcesBusy = true;
	m_DXClass->ResizeResources(size);
	m_ResourcesBusy = false;
}

void RadarPlot::InitGraphics()
{
	for (int i = 0; i != 1024; i++)
	{
		//Vertices[i] = { 2.0f*i / 1024 - 1.0f, 0.0f, 0.0f};
		Vertices[i].X = 2.0f*i / 1024 - 1.0f;
		//Vertices[i].Y = 2.0f*i / 1024 - 1.0f;
		Vertices[i].R = 1.0f;
		Vertices[i].G = 0.604f;
		Vertices[i].B = 0.0f;
	}

	D3D11_BUFFER_DESC buffDesc = { 0 };
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(Vertices);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA srd; // = { Vertices, 0, 0 };
	srd.pSysMem = Vertices;
	srd.SysMemPitch = 0;
	srd.SysMemSlicePitch = 0;

	m_DXClass->GetDevice()->CreateBuffer(&buffDesc, &srd, &m_vertexBuffer);
}

void RadarPlot::InitPipeline()
{
	Array<byte>^ VSFile = RadarPlot::LoadShaderFile("VertexShader.cso");
	Array<byte>^ PSFile = RadarPlot::LoadShaderFile("PixelShader.cso");

	m_DXClass->GetDevice()->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &m_vertexShader);
	m_DXClass->GetDevice()->CreatePixelShader(PSFile->Data, PSFile->Length, nullptr, &m_pixelShader);

	m_DXClass->GetDevContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_DXClass->GetDevContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_DXClass->GetDevice()->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		VSFile->Data,
		VSFile->Length,
		&m_inputLayout
	);

	m_DXClass->GetDevContext()->IASetInputLayout(m_inputLayout.Get());
}

Array<byte>^ RadarPlot::LoadShaderFile(std::string File)
{
	Array<byte>^ FileData = nullptr;

	// open the file
	std::ifstream VertexFile(File, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
	if (VertexFile.is_open())
	{
		// find the length of the file
		int Length = (int)VertexFile.tellg();

		// collect the file data
		FileData = ref new Array<byte>(Length);
		VertexFile.seekg(0, std::ios::beg);
		VertexFile.read(reinterpret_cast<char*>(FileData->Data), Length);
		VertexFile.close();
	}

	return FileData;
}

void RadarPlot::Render()
{
	ID3D11RenderTargetView *const targets[1] = { m_DXClass->GetRenderTargetView() };
	m_DXClass->GetDevContext()->OMSetRenderTargets(1, targets, nullptr);

	float color[4] = { 0.275f, 0.294f, 0.318f, 1.0f };
	m_DXClass->GetDevContext()->ClearRenderTargetView(m_DXClass->GetRenderTargetView(), color);//DirectX::Colors::SkyBlue);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_DXClass->GetDevContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	m_DXClass->GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	m_DXClass->GetDevContext()->Draw(1024, 0);
}

void RadarPlot::Update(unsigned short int Data[4096])
{
	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_DXClass->GetDevContext()->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	VERTEX *ptr = (VERTEX*)resource.pData;
	for (int i = 0; i != 1024; i++)
	{
		//Vertices[i].Y = (data[i]) / 4095;
		ptr[i].X = 2.0f*i / 1024 - 1.0f;
		ptr[i].Y = Data[i] * 0.0005f - 1.0f;//i * 0.00025f;//(data[i]) / 4095;
		ptr[i].Z = 0.0f;
		ptr[i].R = 1.0f;
		ptr[i].G = 0.604f;
		ptr[i].B = 0.0f;

	}

	m_DXClass->GetDevContext()->Unmap(m_vertexBuffer.Get(), 0);
}