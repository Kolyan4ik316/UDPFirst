#include "Game.h"

Game::Game()
{
	client.HintServer("127.0.0.1", 54000);
}
std::string Game::RecivedFromServer() const
{
	return std::string("Server sends: "+ std::string(buffer) + " " + std::to_string(bytesRecived));
}
void Game::Update()
{
	if (slot == 0xFFFF)
	{
		bytesRecived = client.ReceivingMsgs(std::ref(buffer));
		std::cout << bytesRecived << std::endl;
		if (buffer[0] == (char)ServerMessage::Join_Result)
		{
			if (buffer[1] != 0)
			{
				memcpy(&slot, &buffer[2], 2);
				//std::cout << "server assigned us slot " << std::to_string(slot) << std::endl;
			}
			else
			{
				std::cout << "server didn't let us in " << std::endl;
			}
		}
		if (slot == 0xFFFF)
		{
			return;
		}
	}
	
	char input = 0;
	if (kbd.isKeyPressed(sf::Keyboard::W))
	{
		input |= 0x1;
	}
	if (kbd.isKeyPressed(sf::Keyboard::S))
	{
		input |= 0x2;
	}
	if (kbd.isKeyPressed(sf::Keyboard::A))
	{
		input |= 0x4;
	}
	if (kbd.isKeyPressed(sf::Keyboard::D))
	{
		input |= 0x8;
	}
	buffer[0] = (char)ClientMessage::Input;
	memcpy(&slot, &buffer[1], 2);
	buffer[3] = input;
	client.SendingMsgs(std::ref(buffer), 4);

	//bytesRecived = client.ReceivingMsgs(std::ref(buffer));
	//memcpy(&ownSlot, &buffer[readIndex], 2);
	//std::cout << "my slot is " << ownSlot << std::endl;
	//std::cout << "buffer " << buffer << ", packet size: " << packetSize << std::endl;
	int packetSize = client.ReceivingMsgs(std::ref(buffer));
	int readIndex = 1;
	while (readIndex < packetSize)
	{

		readIndex += client.ReadFromBuffer(ownSlot, buffer, readIndex);

		readIndex += client.ReadFromBuffer(playerPosition.x, buffer, readIndex);

		readIndex += client.ReadFromBuffer(playerPosition.y, buffer, readIndex);

		readIndex += client.ReadFromBuffer(playerPosition.facing, buffer, readIndex);

	}
	std::cout << "pos x: " << playerPosition.x << ", pos y: " << playerPosition.y << ", facing: " << playerPosition.facing << std::endl;

}

bool Game::IsRunning() const
{
	return isRunning;
}

void Game::OnEnable()
{
	buffer[0] = (char)ClientMessage::Join;
	client.SendingMsgs(std::ref(buffer), 1);
}

void Game::UnpackingRecBuf()
{
	bytesRecived = client.ReceivingMsgs(std::ref(buffer));
}

void Game::PackingSendBuf()
{
	//std::cin >> sendBuf;
	client.SendingMsgs(std::ref(buffer));
}

void Game::OnDisable()
{
	if (slot != 0xFFFF)
	{
		buffer[0] = (char)ClientMessage::Leave;
		memcpy(&slot, &buffer, 2);
	}
	client.SendingMsgs(std::ref(buffer), 2);
	isRunning = false;
}
