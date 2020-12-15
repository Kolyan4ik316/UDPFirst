#pragma once
#include <WS2tcpip.h>
#include <exception>
#include <string>
#include "Networking.h"

#include <iostream>


#pragma comment (lib, "ws2_32.lib")
using namespace Networking;

class Client
{
public:
	Client(const std::string& ip, const unsigned short& port);
	void HintServer(std::string ip, unsigned short port);
	void ConnectToServer();
	const bool HasConnection() const;
	int ReceivingMsgs(char* recBuf);
	int ReceivingTCPMsgs(char* recBuf);
	int SendingMsgs(char* sendBuf);
	int SendingMsgs(char* sendBuf, int sizeOfBuffer);
	int SendingTCPMsgs(char* sendBuf);
	template<typename T>
	int WriteToBuffer(char* buffer, int index, T& var)
	{
		memcpy(&buffer[index], &var, sizeof(var));
		return sizeof(var);
	}
	
	template<typename T>
	int ReadFromBuffer(T& var, char* buffer, int index)
	{
		memcpy(&var, &buffer[index], sizeof(var));
		return sizeof(var);
	}
	void SelectSocket(SOCKET& sock);
	void Disconnect();
	~Client();
private:
	std::string ip;
	unsigned short port;
	sockaddr_in server;
	SOCKET out;
	SOCKET tcpOut;
	int serverLenght = sizeof(server);
	bool hasConnection = false;
	FD_SET WriteSet;
	FD_SET ReadSet;
	int selectedSock;
};
