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
	char input = recvBuf[0];
	if (input != 0)
	{
		if ((int)input == 1)
		{
			clientPos.y--;
		}
		else if ((int)input == 2)
		{
			clientPos.y++;
		}
		else if ((int)input == 4)
		{
			clientPos.x--;
		}
		else if ((int)input == 8)
		{
			clientPos.x++;
		}
		std::cout << "Position of player is x: " << clientPos.x << ", y: " << clientPos.y << std::endl;
	}
}

void Game::UnpackingRecBuf()
{
	bytesRecived = server.ReceivingMsgs(std::ref(recvBuf));
	
	clientIP = server.PlayerIPString();
	clientPort = server.PlayerPortString();
}

void Game::PackingSendBuf()
{
	//ZeroMemory(sendBuf, 1024);

	server.SendingMsgs(std::ref(sendBuf));
}
