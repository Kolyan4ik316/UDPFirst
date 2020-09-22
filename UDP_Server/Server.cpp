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
}

void Server::ReceivingMsgs(char* recBuf)
{
	ZeroMemory(recBuf, 1024);
	bytesIn = recvfrom(in, recBuf, 1024, 0, (sockaddr*)&client, &clientLenght);
	isReceived = true;
	if (bytesIn == SOCKET_ERROR)
	{
		isReceived = false;
	}
}

void Server::SendingMsgs(char* sendBuf)
{
	//*bufOut = *sendBuf;
	if (bytesOut = sendto(in, sendBuf, 1024, 0, (sockaddr*)&client, clientLenght))
	{
		isSended = true;
	}
	
	if (bytesOut == SOCKET_ERROR)
	{
		isSended = false;
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

Networking::IP_Endpoint Server::GetClientIpPort() const
{
	return Networking::IP_Endpoint({client.sin_addr.S_un.S_addr, client.sin_port});
}

void Server::PlayerIPString(std::string& str)
{
	char clientIp[256];
	inet_ntop(AF_INET, &(client.sin_addr), clientIp, 256);
	str.resize(256);
	str = std::string(clientIp);

}

void Server::PlayerPortString(std::string& str)
{
	std::stringstream ss; 
	ss << client.sin_port;
	str = ss.str();
}

Server::~Server()
{
	// Close the socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}
