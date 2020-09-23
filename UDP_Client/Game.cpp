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
}

void Game::UnpackingRecBuf()
{
	bytesRecived = client.ReceivingMsgs(std::ref(recvBuf));
}

void Game::PackingSendBuf()
{
	std::cin >> sendBuf;
	client.SendingMsgs(std::ref(sendBuf));
}