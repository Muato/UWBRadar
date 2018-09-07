#include "pch.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: Network.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Network.h"



bool tcpip::ConnectToHost(int PortNo, const char* IPAddress)
{
	WSADATA wsadata;
	int error = WSAStartup(0x0202, &wsadata);

	if (error)
		return false;

	//Check if right Winsock version
	if (wsadata.wVersion != 0x0202)
	{
		WSACleanup();
		return false;
	}

	//Fill out init information
	SOCKADDR_IN target;
	target.sin_family = AF_INET;						//address family Internet
	target.sin_port = htons(PortNo);					//Port to connect on
	target.sin_addr.S_un.S_addr = inet_addr(IPAddress);	//Target IP address

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//Create socket
	if (s == INVALID_SOCKET)
		return false;

	//Connect to socket
	if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
	{
		return false;
	}
	else
		return true;
}



void tcpip::CloseConnection()
{
	if (s)
		closesocket(s);
	
	WSACleanup();
}



void tcpip::GetFrame()
{
	//Gets excatly one frame of data from radar
	int result;
	int length = 1024 * 2;
	int len = 0;

	result = send(s, "s", 1, 0);
	//Check if sent excatly one byte
	if(result != 1)
		OutputDebugString(_T("Sending Error happened!"));

	//Read until all bytes are received
	do {
		result = recv(s, (char *)&Data + len, length - len, 0);
		if (result != SOCKET_ERROR)
			len += result;
		else
			break;
	} while (len < length);
	
	DataAcq = true;
}


void tcpip::SendConfigPack(ConfigPack SPack, ConfigPack &RPack)
{
	int result;
	char sendBuff[sizeof(ConfigPack)];
	memcpy(&sendBuff, &SPack, sizeof(ConfigPack));
	
	result = send(s, (const char*)&sendBuff, sizeof(sendBuff), 0);

	if (result != 99)
		OutputDebugString(_T("Didnt send all!"));

	char recBuff[sizeof(ConfigPack)];
	result = recv(s, recBuff, sizeof(ConfigPack), 0);
	if (result != 99)
		OutputDebugString(_T("Didnt receive all!"));

}
