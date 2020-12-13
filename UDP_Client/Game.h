#pragma once

#include "State.h"
#include "Client.h"
#include "FrameTimer.h"
#include "ClientAttributes.h"

#include "GameState.h"


#pragma comment(lib, "Winmm.lib")

/*
	List to do:

	// Client
	Entity class
	Connect this class to player with id
	Ping comparing
	Create graphics class

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
	void InitStates();
	void QuitFromApplication();
public:
	Game();
	void Update();
	void Render();
	void UpdateSFMLEvents();
	void Run();
public:
	void OnEnable();
	void UnpackingRecBuf();
	void PackingSendBuf();
	void OnDisable();
public:
	virtual ~Game();
private:
	std::stack<std::unique_ptr<State>> states;
	std::shared_ptr<sf::RenderWindow> window;
	sf::Event sfEvent;
	std::mutex mtx;
	float dt;
	bool focused = true;
	FrameTimer ft;
	Client client;
	char input = 0;
	unsigned short ownSlot = 0xFFFF;
	char recvBuffer[1024];
	char sendBuffer[1024];
	ClientAttributes player;
	sf::Keyboard kbd;
	sf::Font font;
	sf::Texture texture;
	std::vector<ClientAttributes> otherPlayers;
	static constexpr float timeOut = 15.0f;
	std::string serverAddress;
	unsigned short serverPort;
};