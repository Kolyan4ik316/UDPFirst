#include "Client.h"

Client::Client(const std::string& ip, const unsigned short& port)
	:
	ip(ip),
	port(port)
{
	// Startup winsock
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(version, &data))
	{
		throw(std::exception("WSAStartup failed: " + WSAGetLastError()));
	}
	HintServer(this->ip, this->port);
}

void Client::HintServer(std::string ip, unsigned short port)
{
	// Create a hint structure for the server
	

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	// Socket creation
	out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	tcpOut = socket(AF_INET, SOCK_STREAM, NULL);
	unsigned long enabled = 1;
	ioctlsocket(out, FIONBIO, &enabled);
	ioctlsocket(tcpOut, FIONBIO, &enabled);

	FD_ZERO(&WriteSet);
	FD_ZERO(&ReadSet);

}

void Client::ConnectToServer()
{
	if (connect(tcpOut, (SOCKADDR*)&server, serverLenght) == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		hasConnection = false;
	}
	
	std::cout << "Connected?" << std::endl;
	hasConnection = true;
}

const bool Client::HasConnection() const
{
	return hasConnection;
}

int Client::ReceivingMsgs(char* recBuf)
{
	FD_SET(out, &ReadSet);
	SelectSocket(out);
	ZeroMemory(recBuf, 1024);
	int bytesIn = recvfrom(out, recBuf, 1024, 0, (sockaddr*)&server, &serverLenght);
	if (bytesIn == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	//std::cout << WSAGetLastError();
	return bytesIn;
}

int Client::ReceivingTCPMsgs(char* recBuf)
{
	FD_SET(tcpOut, &ReadSet);
	SelectSocket(tcpOut);
	int bytesIn = recv(tcpOut, recBuf, sizeof(recBuf), 0);
	if (bytesIn == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();;
	}
	return bytesIn;
	
}

int Client::SendingMsgs(char* sendBuf)
{
	ZeroMemory(sendBuf, 1024);
	FD_SET(out, &WriteSet);
	SelectSocket(out);
	int bytesOut = sendto(out, sendBuf, 1024, 0, (sockaddr*)&server, serverLenght);

	if (bytesOut == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	return bytesOut;
}

int Client::SendingMsgs(char* sendBuf, int sizeOfBuffer)
{
	FD_SET(out, &WriteSet);
	SelectSocket(out);
	int bytesOut = sendto(out, sendBuf, sizeOfBuffer, 0, (sockaddr*)&server, serverLenght);

	if (bytesOut == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	return bytesOut;
}

int Client::SendingTCPMsgs(char* sendBuf)
{
	FD_SET(tcpOut, &WriteSet);
	SelectSocket(tcpOut);
	int bytesOut = send(tcpOut, sendBuf, sizeof(sendBuf), 0);
	if (bytesOut == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		return WSAGetLastError();
	}
	return bytesOut;
}

void Client::SelectSocket(SOCKET& sock)
{
	int total;
	if ((total = select(0, &ReadSet, &WriteSet, NULL, NULL)) == SOCKET_ERROR)
	{
		std::cout << "Select return error!" << WSAGetLastError() << std::endl;
	}
	if ((total != 0) && (FD_ISSET(sock, &ReadSet))) // Использую FD_ISSET только для примера! :)
	{
		// Получаю данные
		std::cout << "Taking data!" << std::endl;
	}
	//std::cout << "have total " << total << std::endl;
}

void Client::Disconnect()
{
	// Close the socket
	closesocket(out);
	closesocket(tcpOut);

	// Shutdown winsock
	WSACleanup();
}

Client::~Client()
{
	Disconnect();
}
