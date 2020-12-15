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
	sizeOfSH = sizeof(serverHint);
	unsigned long enabled = 1;
	ioctlsocket(in, FIONBIO, &enabled);
	tcpIn = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(tcpIn, FIONBIO, &enabled);
	if (bind(in, (sockaddr*)&serverHint, sizeOfSH) == SOCKET_ERROR)
	{
		throw(std::exception("Can't bind UDP socket! " + WSAGetLastError()));
	}
	if (bind(tcpIn, (sockaddr*)&serverHint, sizeOfSH) == SOCKET_ERROR)
	{
		throw(std::exception("Can't bind TCP socket! " + WSAGetLastError()));
	}
	listen(tcpIn, 0xFFFFFFFF);
	
	
}

/*void Server::SetAcceptebleSocket(std::shared_ptr<SOCKET> socket)
{
	tcpOut = socket;
}*/


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

void Server::Accept(SOCKET& sock,  sockaddr_in& addr)
{
	int sizeOfadr = sizeof(addr);
	sock = accept(tcpIn, (sockaddr*)&addr, &sizeOfadr);
	if (sock == INVALID_SOCKET && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		std::cout << "Socket error"<< WSAGetLastError() << std::endl;
	}
	
}

bool Server::IsReceived() const
{
	return isReceived;
}

bool Server::IsSended() const
{
	return isSended;
}

int Server::SendingTCPMsgs(char* sendBuf, const SOCKET& sock)
{
	if (send(sock, sendBuf, sizeof(sendBuf), 0) == SOCKET_ERROR && WSAGetLastError() != WSAGetLastError())
	{
		return WSAGetLastError();
	}
	return sizeof(sendBuf);
}

int Server::ReceivingTCPMsgs(char* recBuf, const SOCKET& sock)
{
	if (recv(sock, recBuf, sizeof(recBuf), 0) > 0)
	{
		return sizeof(recBuf);
	}
	return 0;
	
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
	closesocket(tcpIn);

	// Shutdown winsock
	WSACleanup();
}
