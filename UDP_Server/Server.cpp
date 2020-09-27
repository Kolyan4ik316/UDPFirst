#include "Server.h"


Server::Server()
{
	// Startup winsock
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(version, &data))
	{
		throw(std::exception("WSAStartup failed: " + WSAGetLastError()));
	}
}

void Server::Bind(unsigned short port)
{
	// bind socket to ip adress and port
	in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(port);

	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		throw(std::exception("Can't bind socket! " + WSAGetLastError()));
	}
	unsigned long enabled = 1;
	ioctlsocket(in, FIONBIO, &enabled);
}

int Server::ReceivingMsgs(char* recBuf)
{
	ZeroMemory(recBuf, 1024);
	int bytesIn = recvfrom(in, recBuf, 1024, 0, (sockaddr*)&client, &clientLenght);
	isReceived = true;
	if (bytesIn == SOCKET_ERROR)
	{
		isReceived = false;
	}
	return bytesIn;
}

int Server::ReceivingMsgs(char* recBuf, sockaddr_in& from)
{
	ZeroMemory(recBuf, 1024);
	int fromLenght = sizeof(from);
	int bytesIn = recvfrom(in, recBuf, 1024, 0, (sockaddr*)&from, &fromLenght);
	isReceived = true;
	if (bytesIn == SOCKET_ERROR)
	{
		isReceived = false;
	}
	return bytesIn;
}

int Server::SendingMsgs(char* sendBuf, int sizeOfBuffer)
{
	int bytesOut = sendto(in, sendBuf, sizeOfBuffer, 0, (sockaddr*)&client, clientLenght);
	isSended = true;
	
	if (bytesOut == SOCKET_ERROR)
	{
		isSended = false;
	}
	return bytesOut;
}

int Server::SendingMsgs(char* sendBuf, int sizeOfBuffer, sockaddr_in& to)
{
	int toLenght = sizeof(to);
	int bytesOut = sendto(in, sendBuf, sizeOfBuffer, 0, (sockaddr*)&to, toLenght);
	isSended = true;

	if (bytesOut == SOCKET_ERROR)
	{
		isSended = false;
	}
	return bytesOut;
}

bool Server::IsReceived() const
{
	return isReceived;
}

bool Server::IsSended() const
{
	return isSended;
}

IP_Endpoint Server::GetClientIpPort() const
{
	return Networking::IP_Endpoint({client.sin_addr.S_un.S_addr, client.sin_port});
}

std::string Server::PlayerIPString() const
{
	char clientIp[256];
	inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
	return(std::string(clientIp));
}

std::string Server::PlayerPortString() const
{
	return(std::string(std::to_string(GetClientIpPort().port)));
}

sockaddr_in Server::ServerHint() const
{
	return serverHint;
}

sockaddr_in Server::ClientAddress() const
{
	return client;
}

Server::~Server()
{
	// Close the socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}
