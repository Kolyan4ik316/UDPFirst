#include "Game.h"

Game::Game()
{
	client.HintServer("127.0.0.1", 54000);
}
std::string Game::RecivedFromServer() const
{
	return std::string("Server sends: "+ std::string(recvBuf) + " " + std::to_string(bytesRecived));
}
void Game::Update()
{
	char input;
	if (kbd.isKeyPressed(sf::Keyboard::W))
	{
		input |= 0x1;
	}
	else if (kbd.isKeyPressed(sf::Keyboard::S))
	{
		input |= 0x2;
	}
	else if (kbd.isKeyPressed(sf::Keyboard::A))
	{
		input |= 0x4;
	}
	else if (kbd.isKeyPressed(sf::Keyboard::D))
	{
		input |= 0x8;
	}
	else
	{
		input = 0;
	}
	sendBuf[0] = input;
}

void Game::UnpackingRecBuf()
{
	bytesRecived = client.ReceivingMsgs(std::ref(recvBuf));
}

void Game::PackingSendBuf()
{
	//std::cin >> sendBuf;
	client.SendingMsgs(std::ref(sendBuf));
}