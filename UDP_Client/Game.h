#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Vec2.h"
#include "Client.h"
#include <vector>
#include <string>
#include "FrameTimer.h"
#include <mutex>
#include "ClientAttributes.h"

#pragma comment(lib, "Winmm.lib")

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
	char recvBuffer[1024];
	char sendBuffer[1024];
	bool isRunning = true;
	char input;
	sf::Keyboard kbd;
	ClientAttributes player;
	sf::Font font;
	sf::Texture texture;
	std::vector<ClientAttributes> otherPlayers;
	static constexpr float timeOut = 15.0f;
};