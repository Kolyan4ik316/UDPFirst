#pragma once
#include <WS2tcpip.h>
#include <exception>
#include <string>
#include <sstream>

#include "Networking.h"

#pragma comment (lib, "ws2_32.lib")

class Server
{
public:
	Server();
	void Bind(unsigned short port);
	void ReceivingMsgs(char* recBuf);
	void SendingMsgs(char* sendBuf);
	bool IsReceived() const;
	bool IsSended() const;
	Networking::IP_Endpoint GetClientIpPort() const;
	void PlayerIPString(std::string& str);
	void PlayerPortString(std::string& str);
	~Server();
private:
	SOCKET in;
	sockaddr_in serverHint;
	char buf[1024];
	char bufOut[1024];
	sockaddr_in client;
	int clientLenght = sizeof(client);
	int bytesIn;
	int bytesOut;
	bool isReceived = false;
	bool isSended = false;
};

