//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Content\RadarPlot.h"
#include "Common\D3D11class.h"

struct ListItem
{
	String ^Name;
	short int Value;
};

namespace UWBRadar
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		std::unique_ptr<RadarPlot> m_radar;
		//std::unique_ptr<MessageManager> m_MsgMgr;
		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

		void RunButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void UpdateMessage(Platform::String ^ msg, float time);
		void UpdateConfigs();

		Windows::Foundation::IAsyncAction^ m_TCPIPworker;


		String ^TextRT;
		String ^Message;
		int messageTimer;
		bool m_running;
		bool FailedToConnect = false;
		bool ConfigsChanged = false;

		// Configuration limits
		const int limScale[2] = { 0, 1023 };
		const int limDelay[2] = { 0, 2047 };
		const float limRecursive[2] = { 0.0f , 1.0f };
		const int limStart[2] = { 0, 4094 };
		const int limStop[2] = { 1, 4095 };
		const int limLength[2] = { 1, 4096 };
		const int limTimeStep[2] = { 1, 4095 };

		short int Steps[40] = { 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

		ConfigPack confPack; //TODO
		void UI_ConfigsChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	};
}

