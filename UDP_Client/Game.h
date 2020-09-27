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
	bool IsRunning()const;
public:
	void OnEnable();
	void UnpackingRecBuf();
	void PackingSendBuf();
	void OnDisable();
private:
	FrameTimer ft;
	//std::vector<Vec2> clientsPos;
	Vec2 clientPos;
	Client client;
	unsigned short ownSlot;
	PlayerState playerPosition = {};
	char buffer[1024];
	int bytesRecived;
	bool isRunning = true;
	bool isSended0msg = false;
	unsigned short slot = 0xFFFF;
	bool inDestruction;
	sf::Keyboard kbd;
	//int bytesSended;

};