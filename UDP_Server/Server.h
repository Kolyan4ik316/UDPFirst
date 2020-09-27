#pragma once
#include <WS2tcpip.h>
#include <exception>
#include <string>
#include <map>


#include "Networking.h"

#pragma comment (lib, "ws2_32.lib")
using namespace Networking;

class Server
{
public:
	Server();
	void Bind(unsigned short port);
	int ReceivingMsgs(char* recBuf);
	int ReceivingMsgs(char* recBuf, sockaddr_in& from);
	int SendingMsgs(char* sendBuf, int sizeOfBuffer);
	int SendingMsgs(char* sendBuf, sockaddr_in& to);
	int SendingMsgs(char* sendBuf, int sizeOfBuffer, sockaddr_in& to);
	bool IsReceived() const;
	bool IsSended() const;
	IP_Endpoint GetClientIpPort() const;
	std::string PlayerIPString() const;
	std::string PlayerPortString() const;
	sockaddr_in ServerHint() const;
	sockaddr_in ClientAddress() const;
	~Server();
private:
	SOCKET in;
	sockaddr_in serverHint;
	sockaddr_in client;
	sockaddr_in to;
	int clientLenght = sizeof(client);
	bool isReceived = false;
	bool isSended = false;
	
	
};

