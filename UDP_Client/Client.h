#pragma once
#include <WS2tcpip.h>
#include <exception>
#include <string>
#include "Networking.h"


#pragma comment (lib, "ws2_32.lib")
using namespace Networking;

class Client
{
public:
	Client();
	void HintServer(std::string ip, unsigned short port);
	int ReceivingMsgs(char* recBuf);
	int SendingMsgs(char* sendBuf);
	int SendingMsgs(char* sendBuf, int sizeOfBuffer);
	
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
	~Client();
private:
	sockaddr_in server;
	SOCKET out;
	int serverLenght = sizeof(server);
};
