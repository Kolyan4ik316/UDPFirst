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


int Server::ReceivingMsgs(char* recBuf, sockaddr_in& from)
{
	ZeroMemory(recBuf, sizeof(recBuf));
	int fromLenght = sizeof(from);
	int bytesIn = recvfrom(in, recBuf, sizeOfMsgs, 0, (sockaddr*)&from, &fromLenght);
	isReceived = true;
	if (bytesIn == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		isReceived = false;
		return WSAGetLastError();
	}
	return bytesIn;
}

int Server::SendingMsgs(char* sendBuf, int sizeOfBuffer, sockaddr_in& to)
{
	int toLenght = sizeof(to);
	int bytesOut = sendto(in, sendBuf, sizeOfBuffer, 0, (sockaddr*)&to, toLenght);
	isSended = true;

	if (bytesOut == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		isSended = false;
		return WSAGetLastError();
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

SOCKET Server::GetSocket()
{
	return in;
}

sockaddr_in Server::GetServerHint() const
{
	return serverHint;
}

void Server::SetSizeOfMsgs(int num)
{
	int sizeOfMsgs = num;
}

Server::~Server()
{
	// Close the socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}
