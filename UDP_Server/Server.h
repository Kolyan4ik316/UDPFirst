#pragma once
#include <WS2tcpip.h>
#include <exception>


#include "Vec2.h"
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
	~Server();
private:
	SOCKET in;
	sockaddr_in serverHint;
	Networking::IP_Endpoint clientIP_Endpoint;
	Vei2 clientPos;
	char buf[1024];
	char bufOut[1024];
	sockaddr_in client;
	int clientLenght = sizeof(client);
	int bytesIn;
	int bytesOut;
	bool isReceived = false;
	bool isSended = false;
};

