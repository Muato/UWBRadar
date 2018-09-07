//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace UWBRadar;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::System::Threading;
using namespace Windows::UI::Core;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	m_running = false;
	InitializeComponent();

	// Register event handlers
	swapChainPanel->SizeChanged +=
		ref new SizeChangedEventHandler(this, &MainPage::OnSwapChainPanelSizeChanged);

	TimeSpan timer;
	timer.Duration = 1000000; // Every 0.1 sec

	ThreadPoolTimer ^MessageTimer = ThreadPoolTimer::CreatePeriodicTimer(
		ref new TimerElapsedHandler([this](ThreadPoolTimer ^source)
	{
		Dispatcher->RunAsync(CoreDispatcherPriority::High,
			ref new DispatchedHandler([this]()
		{
			if (FailedToConnect == true)
			{
				RunButton->Content = "Start";
				m_running = false;
				RunButton->IsChecked = false;
				FailedToConnect = false;
			}

			if (messageTimer == 1)
			{
				MessageBox->Text = " ";
				messageTimer--;
			}
			else if (messageTimer > 1)
				MessageBox->Text = Message;
				messageTimer--;

		}));
		
	}), timer);

	UpdateMessage("Application started!",2);

	m_radar = std::unique_ptr<RadarPlot>(new RadarPlot);
	m_radar->SetExampleSwapChainPanel(swapChainPanel);
	m_radar->Initialize();
	m_radar->Run();

}

void MainPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	m_radar->Resize(e->NewSize);
}



void UWBRadar::MainPage::RunButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!m_running)
	{
		RunButton->Content = "Stop";
		m_running = true;
		UpdateMessage("Connecting...", 15);

		TextRT = IP_TexBox->Text;
		UpdateConfigs();
			
		auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
		{
			std::wstring TextW(TextRT->Begin());
			std::string TextA(TextW.begin(), TextW.end());
			const char* charStr = TextA.c_str();
			if (m_radar->m_tcpip->ConnectToHost(5000, charStr))
			{
				
				ConfigPack testPack;

				m_radar->m_tcpip->SendConfigPack(confPack, testPack);
				UpdateMessage("Connected", 2);
				while (action->Status == AsyncStatus::Started)
				{
					if (ConfigsChanged)
					{
						m_radar->m_tcpip->SendConfigPack(confPack, testPack);
						ConfigsChanged = false;
					}
					m_radar->m_tcpip->GetFrame();
				}
			}
			else
			{
				UpdateMessage("Connection failed", 2);
				FailedToConnect = true;
				

			}
		});
		m_TCPIPworker = ThreadPool::RunAsync(workItemHandler);
		
	}

	else
	{
		m_TCPIPworker->Cancel();

		RunButton->Content = "Start";
		UpdateMessage("Disconnected", 2);
		m_running = false;

		m_radar->m_tcpip->CloseConnection();
	}
}

void UWBRadar::MainPage::UpdateMessage(Platform::String ^ msg, float time)
{
	messageTimer = 10 * time;
	Message = msg;
}

void UWBRadar::MainPage::UpdateConfigs()
{
	int Scale, Delay, Start, Stop, Length, TimeStep;
	float Recursive;
	Scale = _wtoi(ScaleBox->Text->Data());
	Delay = _wtoi(DelayBox->Text->Data());
	Recursive = _wtof(RecursiveBox->Text->Data());
	Start = _wtoi(StartBox->Text->Data());
	Stop = _wtoi(StopBox->Text->Data());
	Length = _wtoi(LengthBox->Text->Data());
	TimeStep = _wtoi(TimeStepBox->Text->Data());

	confPack.Type[0] = 'c';
	confPack.Type[1] = 1;
	(short int&)confPack.Scale = (short int)Scale;
	(short int&)confPack.Delay = (short int)Delay;
	(float &)confPack.Recursive = Recursive;
	(short int&)confPack.Start = (short int)Start;
	(short int&)confPack.Stop = (short int)Stop;
	(short int&)confPack.Length = (short int)Length;
	(short int&)confPack.TimeStep = (short int)TimeStep;
	(short int&)confPack.StepCount = 19;

	
	for (int i = 0; i < 40; i++)
	{
		(short int&)confPack.Steps[i*2] = Steps[i];
	}

	

	//return confPack;
}

void UWBRadar::MainPage::UI_ConfigsChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Enter)
	{
		int Scale = _wtoi(ScaleBox->Text->Data());
		int Delay = _wtoi(DelayBox->Text->Data());
		float Recursive = _wtof(RecursiveBox->Text->Data());
		int Start = _wtoi(StartBox->Text->Data());
		int Stop = _wtoi(StopBox->Text->Data());
		int Length = _wtoi(LengthBox->Text->Data());
		int TimeStep = _wtoi(TimeStepBox->Text->Data());
		
		if (sender->GetHashCode() == ScaleBox->GetHashCode())
		{
			// Chek ScaleBox limits
			if (Scale > limScale[1])
				Scale = limScale[1];
			else if (Scale < limScale[0])
				Scale = limScale[0];
		}
		else if (sender->GetHashCode() == DelayBox->GetHashCode())
		{
			// Chek ScaleBox limits
			if (Delay > limDelay[1])
				Delay = limDelay[1];
			else if (Delay < limDelay[0])
				Delay = limDelay[0];
		}
		else if (sender->GetHashCode() == RecursiveBox->GetHashCode())
		{
			// Chek ScaleBox limits
			if (Recursive > limRecursive[1])
				Recursive = limRecursive[1];
			else if (Recursive < limRecursive[0])
				Recursive = limRecursive[0];
		}
		else if (sender->GetHashCode() == StartBox->GetHashCode())
		{
			// Check StartBox limits
			if (Start > limStart[1])
				Start = limStart[1];
			else if (Start < limStart[0])
				Start = limStart[0];
			
			int temp = Start + (Length - 1) * TimeStep;
			if (temp > limStop[1])
			{
				Length = (floor((limStop[1] - Start) / TimeStep) + 1);
				Stop = Start + (Length - 1) * TimeStep;
			}
			else
				Stop = temp;
		}
		else if (sender->GetHashCode() == StopBox->GetHashCode())
		{
			// Check StopBox limits
			if (Stop > limStop[1])
				Stop = limStop[1];
			else if (Stop < limStop[0])
				Stop = limStop[0];

			int temp = (Stop - Start + TimeStep) / TimeStep;
			Stop = floor(temp) * TimeStep + Start - TimeStep;
			Length = floor(temp);
		}
		else if (sender->GetHashCode() == LengthBox->GetHashCode())
		{
			// Check LengthBox limits
			if (Length > limLength[1])
				Length = limLength[1];
			else if (Length < limLength[0])
				Length = limLength[0];

			int temp = Start + (Length - 1) * TimeStep;
			if (temp > limStop[1])
				Length = floor((limStop[1] - Start + TimeStep) / TimeStep);
			Stop = Start + (Length - 1) * TimeStep;
		}
		else if (sender->GetHashCode() == TimeStepBox->GetHashCode())
		{
			// Check TimeStepBox limits
			if (TimeStep > limTimeStep[1])
				TimeStep = limTimeStep[1];
			else if (TimeStep < limTimeStep[0])
				TimeStep = limTimeStep[0];

			int temp = Start + (Length - 1) * TimeStep;
			if (temp > limStop[1])
			{
				Length = floor((limStop[1] - Start) / TimeStep);
				Stop = Start + Length * TimeStep;
			}
			else
				Stop = temp;
		}

		StartBox->Text = Start.ToString();
		StopBox->Text = Stop.ToString();
		LengthBox->Text = Length.ToString();
		TimeStepBox->Text = TimeStep.ToString();

		UpdateConfigs();
		ConfigsChanged = true;
		UpdateMessage("Configurations updated", 2);
	}
}
