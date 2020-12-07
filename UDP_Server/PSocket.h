#pragma once
#include "Networking.h"
struct PSocket
{
	SOCKET socket;
	char recvBuffer[1024];
	char sendBuffer[1024];
	Networking::IP_Endpoint fromEndpoint;
};