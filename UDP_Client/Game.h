#pragma once
#include "Vec2.h"
#include "Client.h"
#include <vector>
#include <string>
#include <iostream>
#include "FrameTimer.h"

class Game
{
public:
	Game();
	std::string RecivedFromServer() const;
	void Update();
	//~Game() = default;
public:
	void UnpackingRecBuf();
	void PackingSendBuf();
private:
	FrameTimer ft;
	std::vector<Vec2> clientsPos;
	Client client;
	char recvBuf[1024];
	char sendBuf[1024];
	int bytesRecived;
	//int bytesSended;

};