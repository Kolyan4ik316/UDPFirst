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

	unsigned long enabled = 1;
	ioctlsocket(out, FIONBIO, &enabled);
}

void Client::HintServer(std::string ip, unsigned short port)
{
	// Create a hint structure for the server
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	// Socket creation
	out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

int Client::ReceivingMsgs(char* recBuf)
{
	ZeroMemory(recBuf, 1024);
	int bytesIn = recvfrom(out, recBuf, 1024, 0, (sockaddr*)&server, &serverLenght);
	isReceived = true;
	if (bytesIn == SOCKET_ERROR)
	{
		isReceived = false;
	}
	return bytesIn;
}

int Client::SendingMsgs(char* sendBuf)
{
	int bytesOut = sendto(out, sendBuf, 1024, 0, (sockaddr*)&server, serverLenght);
	isSended = true;

	if (bytesOut == SOCKET_ERROR)
	{
		isSended = false;
	}
	return bytesOut;
}

int Client::SendingMsgs(char* sendBuf, int sizeOfBuffer)
{
	int bytesOut = sendto(out, sendBuf, sizeOfBuffer, 0, (sockaddr*)&server, serverLenght);
	isSended = true;

	if (bytesOut == SOCKET_ERROR)
	{
		isSended = false;
	}
	return bytesOut;
}

bool Client::IsReceived() const
{
	return isReceived;
}

bool Client::IsSended() const
{
	return isSended;
}

Client::~Client()
{
	// Close the socket
	closesocket(out);

	// Shutdown winsock
	WSACleanup();
}
