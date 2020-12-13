#pragma once

#include "State.h"
#include "Client.h"
#include "FrameTimer.h"

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
	virtual ~Game();
private:
	std::stack<std::unique_ptr<State>> states;
	std::shared_ptr<sf::RenderWindow> window;
	std::shared_ptr<Client> client;
	sf::Event sfEvent;
	std::mutex mtx;
	float dt;
	bool focused = true;
	FrameTimer ft;
	unsigned short ownSlot = 0xFFFF;
	std::string serverAddress;
	unsigned short serverPort;
};