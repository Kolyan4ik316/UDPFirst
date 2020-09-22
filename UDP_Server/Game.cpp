#include "Game.h"



Game::Game()
{
	server.Bind(54000);
}
std::string Game::RecivedFromClient() const
{
	return std::string(std::string(clientIP) + " " + std::string(clientPort) + ": " + std::string(recvBuf));
}
void Game::Update()
{
}

void Game::UnpackingRecBuf()
{
	server.ReceivingMsgs(std::ref(recvBuf));
	server.PlayerIPString(clientIP);
	server.PlayerPortString(clientPort);
}

void Game::PackingSendBuf()
{
	server.SendingMsgs(std::ref(sendBuf));
}
