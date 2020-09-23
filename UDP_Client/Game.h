#pragma once
#include "Vec2.h"
#include "Client.h"
#include <vector>
#include <string>
#include <iostream>
#include "FrameTimer.h"

#include <SFML/Window/Keyboard.hpp>

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
	Vec2 clientPos;
	Client client;
	char recvBuf[1024];
	char sendBuf[1024];
	int bytesRecived;
	sf::Keyboard kbd;
	//int bytesSended;

};