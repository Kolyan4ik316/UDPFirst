#pragma once
#include "Server.h"
#include <vector>
#include <string>
#include "FrameTimer.h"
#include <iostream>
#include <mutex>
#include <map>

class Game
{
public:
	Game();
	void Update(std::mutex& mtx);
	bool IsRunning()const;
	//~Game() = default;
public:
	void AcceptConnection(std::mutex& mtx);
	void UnpackingRecBuf(std::mutex& mtx);
	void PackingSendBuf(std::mutex& mtx);
private:
	FrameTimer ft;
	Server server;
	char recvBuffer[1024];
	char sendBuffer[1024];
	bool isRunning = true;
	//static constexpr float deltaTime = 1.0f / 60.0f;
	static constexpr float maxSpeed = 50.0f;
	static constexpr float clientTimeOut = 15.0f;
	static constexpr float acceleration = 25.0f;
	static constexpr unsigned short tempElement = 0xFFFF;
private:
	//std::shared_ptr<SOCKET> outSock;
	SOCKET outSock;
	std::map<unsigned short, ClientAttributes> clientAttr;
	float timeForSending = 0;
	static constexpr float timeGranted = 1.0f / 120.0f;
};
