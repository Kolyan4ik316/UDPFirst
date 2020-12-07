#pragma once

#include <exception>
#include <string>
#include <map>

#include "Networking.h"



using namespace Networking;

class Server
{
public:
	Server();
	void Bind(unsigned short port);
	int ReceivingMsgs(char* recBuf, sockaddr_in& from);
	int SendingMsgs(char* sendBuf, int sizeOfBuffer, sockaddr_in& to);
	bool IsReceived() const;
	bool IsSended() const;
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
	~Server();
private:
	SOCKET in;
	sockaddr_in serverHint;
	bool isReceived = false;
	bool isSended = false;
	
	
};


