#include "Client.h"

Client::Client()
{
	// Startup winsock
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(version, &data))
	{
		throw(std::exception("WSAStartup failed: " + WSAGetLastError()));
	}
}

void Client::HintServer(std::string ip, unsigned short port)
{
	// Create a hint structure for the server
	

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	// Socket creation
	out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	unsigned long enabled = 1;
	ioctlsocket(out, FIONBIO, &enabled);
}

int Client::ReceivingMsgs(char* recBuf)
{
	ZeroMemory(recBuf, 1024);
	int bytesIn = recvfrom(out, recBuf, 1024, 0, (sockaddr*)&server, &serverLenght);
	if (bytesIn == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	return bytesIn;
}

int Client::SendingMsgs(char* sendBuf)
{
	int bytesOut = sendto(out, sendBuf, 1024, 0, (sockaddr*)&server, serverLenght);

	if (bytesOut == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	return bytesOut;
}

int Client::SendingMsgs(char* sendBuf, int sizeOfBuffer)
{
	int bytesOut = sendto(out, sendBuf, sizeOfBuffer, 0, (sockaddr*)&server, serverLenght);

	if (bytesOut == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	return bytesOut;
}

void Client::Disconnect()
{
	// Close the socket
	closesocket(out);

	// Shutdown winsock
	WSACleanup();
}

Client::~Client()
{
	Disconnect();
}
