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
	bool IsReceived() const;
	bool IsSended() const;
	~Client();
private:
	sockaddr_in server;
	SOCKET out;
	int serverLenght = sizeof(server);
	char buf[1024];
	char bufOut[1024];
	bool isReceived = false;
	bool isSended = false;
	unsigned short slot = 0xFFFF;
};
