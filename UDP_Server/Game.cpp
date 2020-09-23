#include "Game.h"



Game::Game()
{
	server.Bind(54000);
}
std::string Game::RecivedFromClient() const
{
	return std::string(clientIP + " " + clientPort + ": " + std::string(recvBuf) + " bytes recived = " + std::to_string(bytesRecived));
}
void Game::Update()
{
}

void Game::UnpackingRecBuf()
{
	bytesRecived = server.ReceivingMsgs(std::ref(recvBuf));
	if (recvBuf[0] != 0)
	{
		std::cout << RecivedFromClient() << std::endl;
	}
	clientIP = server.PlayerIPString();
	clientPort = server.PlayerPortString();
}

void Game::PackingSendBuf()
{
	//ZeroMemory(sendBuf, 1024);
	server.SendingMsgs(std::ref(sendBuf));
}
