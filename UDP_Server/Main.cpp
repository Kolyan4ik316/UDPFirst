#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main()
{
	// Startup winsock
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(version, &data))
	{
		std::cout<<"WSAStartup failed: "<< WSAGetLastError()<<std::endl;
		return - 1;
	}

	// bind socket to ip adress and port
	SOCKET in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(54000);

	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
		return -2;
	}
	
	sockaddr_in client;
	int clientLenght = sizeof(client);
	ZeroMemory(&client, clientLenght);
	

	char buf[1024];
	
	bool isRunning = true;
	// enter a loop
	while(isRunning)
	{ 
		ZeroMemory(buf, 1024);
		// Wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLenght);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receving from client " << WSAGetLastError << std::endl;
			continue;
		}
		// Display message and client info
		char clientIp[256];
		ZeroMemory(clientIp, 256);
		//char clientPort[128];
		//ZeroMemory(clientPort, 128);
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
		//inet_ntop(AF_INET, &client.sin_port, clientIp, 128);

		std::cout << "Message recived from " << clientIp << ":" << client.sin_port << "\n" << buf << std::endl;
		
	}
	// close socket
	closesocket(in);
	// shutdown winsock
	WSACleanup();

	return 0;
}