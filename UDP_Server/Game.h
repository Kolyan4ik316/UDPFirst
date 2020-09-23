#pragma once
#include "Vec2.h"
#include "Server.h"
#include <vector>
#include <string>
#include "FrameTimer.h"
#include <iostream>

class Game
{
public:
	Game();
	std::string RecivedFromClient() const;
	void Update();
	//~Game() = default;
public:
	void UnpackingRecBuf();
	void PackingSendBuf();
private:
	FrameTimer ft;
	std::vector<Vec2> clientsPos;
	std::vector<Networking::IP_Endpoint> clientIP_Endpoint;
	Server server;
	char recvBuf[1024];
	char sendBuf[1024] = "Sock it!";
	std::string clientIP;
	std::string clientPort;
	int bytesRecived;
};
