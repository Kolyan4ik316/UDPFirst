#pragma once
#include "Vec2.h"
#include "Server.h"
#include <vector>
#include <string>
#include "FrameTimer.h"
#include <iostream>
#include <mutex>

class Game
{
public:
	Game();
	std::string RecivedFromClient() const;
	void Update();
	bool IsRunning()const;
	//~Game() = default;
public:
	void UnpackingRecBuf();
	void PackingSendBuf();
private:
	FrameTimer ft;
	//std::vector<Vec2> clientsPos;
	//Vec2 clientPos;
	//std::vector<Networking::IP_Endpoint> clientIP_Endpoint;
	Server server;
	char recvBuffer[1024];
	char sendBuffer[1024];
	std::string clientIP;
	std::string clientPort;
	bool isRunning = true;
	int bytesReceived;
	//static constexpr float deltaTime = 1.0f / 60.0f;
	static constexpr float maxSpeed = 50.0f;
	static constexpr float clientTimeOut = 5.0f;
	static constexpr float acceleration = 20.0f;
	static constexpr float turnSpeed = 1.0f;
private:
	static constexpr unsigned short Max_Clients = 32;
	IP_Endpoint clientEndpoints[Max_Clients];
	PlayerState clientObjects[Max_Clients];
	PlayerInput clientInputs[Max_Clients];
	IP_Endpoint fromEndpoint;
	float time_since_heard_from_clients[Max_Clients];
};
