#pragma once
#include <WS2tcpip.h>
#include <exception>
#include <string>


#include "Networking.h"

#pragma comment (lib, "ws2_32.lib")

class Server
{
public:
	Server();
	void Bind(unsigned short port);
	int ReceivingMsgs(char* recBuf);
	int SendingMsgs(char* sendBuf);
	bool IsReceived() const;
	bool IsSended() const;
	Networking::IP_Endpoint GetClientIpPort() const;
	std::string PlayerIPString() const;
	std::string PlayerPortString() const;
	~Server();
private:
	SOCKET in;
	sockaddr_in serverHint;
	char buf[1024];
	char bufOut[1024];
	sockaddr_in client;
	int clientLenght = sizeof(client);
	bool isReceived = false;
	bool isSended = false;
};

