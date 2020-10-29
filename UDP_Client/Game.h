#pragma once
#include <SFML/Window/Keyboard.hpp>
#include "Vec2.h"
#include "Client.h"
#include <vector>
#include <string>
#include <iostream>
#include "FrameTimer.h"
#include <mutex>


class Game
{
public:
	Game();
	std::string RecivedFromServer() const;
	void Update(std::mutex& mtx);
	//~Game() = default;
	bool IsRunning()const;
public:
	void OnEnable();
	void UnpackingRecBuf(std::mutex& mtx);
	void PackingSendBuf(std::mutex& mtx);
	void OnDisable();
private:
	FrameTimer ft;
	//std::vector<Vec2> clientsPos;
	Vec2 clientPos;
	Client client;
	unsigned short ownSlot = 0xFFFF;
	std::vector<unsigned short> slots;
	std::vector<PlayerState> clientObjects;
	PlayerState localPlayer;
	char recvBuffer[1024];
	char sendBuffer[1024];
	int bytesRecived;
	bool isRunning = true;
	//unsigned short slot ;
	bool inDestruction;
	sf::Keyboard kbd;
	//int bytesSended;

};