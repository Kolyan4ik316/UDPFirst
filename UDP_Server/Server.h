#pragma once

#include <exception>
#include <string>
#include <map>
#include <memory>

#include <iostream>

#include "Networking.h"



using namespace Networking;

class Server
{
public:
	Server();
	void Bind(unsigned short port);
	//void SetAcceptebleSocket(std::shared_ptr<SOCKET> socket);
	int ReceivingMsgs(char* recBuf, sockaddr_in& from);
	int SendingMsgs(char* sendBuf, int sizeOfBuffer, sockaddr_in& to);
	void Accept(SOCKET& sock ,sockaddr_in& addr);
	bool IsReceived() const;
	bool IsSended() const;
	int SendingTCPMsgs(char* sendBuf, const SOCKET& sock);
	int ReceivingTCPMsgs(char* recBuf, const SOCKET& sock);
	SOCKET GetSocket();
	sockaddr_in GetServerHint() const;
	template<typename T>
	int WriteToBuffer(char* buffer, int index,T& var)
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
	void SetSizeOfMsgs(int num);
	~Server();
private:
	int sizeOfSH;
	SOCKET in;
	sockaddr_in serverHint;
	SOCKET tcpIn;
	//std::shared_ptr<SOCKET> tcpOut;
	bool isReceived = false;
	bool isSended = false;
	int sizeOfMsgs = 1024;
	
	
};


