#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>
#include <mutex>

#include<iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Client.h"

class State
{
public:
	State(std::shared_ptr<sf::RenderWindow> window);
	virtual void Update(const float& dt) = 0;
	virtual void Render(sf::RenderTarget* target = nullptr) = 0;
	virtual void CheckForQuit();
	virtual void UpdateInput(const float& dt) = 0;
	virtual void FocusedState(const bool& isFocused);
	virtual void EndState() = 0;
	virtual void UnpackMsg(Client& client, std::mutex& mtx) = 0;
	virtual void PackMsg(Client& client, std::mutex& mtx) = 0;

	const bool& GetQuit() const;

	virtual ~State();
protected:
	const bool& IsFocused() const;
	std::shared_ptr<sf::RenderWindow> window;
	char recvBuffer[1024];
	char sendBuffer[1024];
	unsigned short ownSlot = 0XFFFF;
	sf::Font font;
private:
	bool quit = false;
	bool focused = true;
};