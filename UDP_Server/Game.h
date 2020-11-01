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
	void Update(std::mutex& mtx);
	bool IsRunning()const;
	//~Game() = default;
public:
	void UnpackingRecBuf(std::mutex& mtx);
	void PackingSendBuf(std::mutex& mtx);
private:
	FrameTimer ft;
	Server server;
	char recvBuffer[1024];
	char sendBuffer[1024];
	std::string clientIP;
	std::string clientPort;
	bool isRunning = true;
	//static constexpr float deltaTime = 1.0f / 60.0f;
	static constexpr float maxSpeed = 50.0f;
	static constexpr float clientTimeOut = 5.0f;
	static constexpr float acceleration = 25.0f;
	static constexpr float turnSpeed = 0.5f;
private:
	static constexpr unsigned short Max_Clients = 32;
	std::vector<IP_Endpoint> clientEndpoints;
	std::vector<PlayerState> clientObjects;
	std::vector<PlayerInput> clientInputs;
	std::vector<float> time_since_heard_from_clients;
	std::vector<unsigned short> slots;
	unsigned short errorSlot;
};
