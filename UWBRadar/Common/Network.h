#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: Network.h
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <tchar.h>

struct ConfigPack {
	char Type[2];
	char Delay[2];
	char Scale[2];
	char Start[2];
	char Stop[2];
	char Length[2];
	char TimeStep[2];
	char Recursive[4];
	char StepCount;
	char Steps[80];

};

class tcpip
{
public:
	bool ConnectToHost(int PortNo, const char* IPAddress);
	void CloseConnection();
	void GetFrame();
	void SendConfigPack(ConfigPack Pack, ConfigPack &RPack);
	void RecConfigPack(ConfigPack &Pack);

	unsigned short int Data[4096] = {0};
	bool DataAcq = false;

private:
	SOCKET s;
	
};