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
		std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
		return -1;
	}
	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(54000);

	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
	// Socket creation
	SOCKET out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Write out to that socket
	char buf[1024];
	bool isRunning = true;
	while (isRunning)
	{
		std::cin >> buf;
		if (buf == "quit")
		{
			isRunning = false;
		}
		ZeroMemory(buf, 1024);
		int sendOk = sendto(out, buf, 1024, 0, (sockaddr*)&server, sizeof(server));
		
	}
	// Close the socket
	closesocket(out);

	// Shutdown winsock
	WSACleanup();
	return 0;
}