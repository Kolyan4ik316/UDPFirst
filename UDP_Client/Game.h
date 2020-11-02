#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Vec2.h"
#include "Client.h"
#include <vector>
#include <string>
#include <iostream>
#include "FrameTimer.h"
#include <mutex>

/*
	List to do:

	// Client
	Entity class
	Connect this class to player with id
	Ping comparing
	Create graphics class
	Player will drawing as Circle Green, other players will be Red!

	// Server
	Create client class which including
	slot
	clientEndpoint
	clientObject
	clientInput
	time_since_heard_from_client
	Later we can take name, color, etc
	Compare if slot is already exsits
	
*/

class Game
{
public:
	Game();
	std::string RecivedFromServer() const;
	void Update(std::mutex& mtx, sf::RenderWindow& window);
	//~Game() = default;
	bool IsRunning()const;
public:
	void OnEnable(std::mutex& mtx);
	void UnpackingRecBuf(std::mutex& mtx);
	void PackingSendBuf(std::mutex& mtx, bool& focused);
	void OnDisable(std::mutex& mtx);
private:
	FrameTimer ft;
	Client client;
	unsigned short ownSlot = 0xFFFF;
	std::vector<unsigned short> slots;
	std::vector<PlayerState> clientObjects;
	PlayerState localPlayer;
	char recvBuffer[1024];
	char sendBuffer[1024];
	int bytesRecived;
	bool isRunning = true;
	char input;
	bool inDestruction;
	sf::Keyboard kbd;
	sf::RectangleShape player;
	std::vector<sf::RectangleShape> otherPlayers;
	float timeOut = 0.0f;
};