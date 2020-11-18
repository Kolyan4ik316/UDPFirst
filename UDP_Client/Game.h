#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Vec2.h"
#include "Client.h"
#include <vector>
#include <string>
#include "FrameTimer.h"
#include <mutex>
#include "ClientAttributes.h"
#include <memory>

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
private:
	void InitWindow();
public:
	Game();
	void Update();
	void Render();
	void UpdateSFMLEvents();
	void Run();
	virtual ~Game();
	bool IsRunning()const;
public:
	void OnEnable();
	void UnpackingRecBuf();
	void PackingSendBuf();
	void OnDisable();
private:
	std::unique_ptr<sf::RenderWindow> window;
	sf::Event sfEvent;
	std::mutex mtx;
	bool focused = true;
	FrameTimer ft;
	Client client;
	char input = 0;
	unsigned short ownSlot = 0xFFFF;
	char recvBuffer[1024];
	char sendBuffer[1024];
	bool isRunning = true;
	ClientAttributes player;
	sf::Keyboard kbd;
	sf::Font font;
	sf::Texture texture;
	std::vector<ClientAttributes> otherPlayers;
	static constexpr float timeOut = 15.0f;
};