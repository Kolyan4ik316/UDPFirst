#include "Game.h"

Game::Game()
{
	client.HintServer("127.0.0.1", 54000);
}
std::string Game::RecivedFromServer() const
{
	return std::string("Server sends: "+ std::string(recvBuffer) + " " + std::to_string(bytesRecived));
}
void Game::Update(std::mutex& mtx)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
	if (kbd.isKeyPressed(sf::Keyboard::Q))
	{
		OnDisable();
	}
	

}

bool Game::IsRunning() const
{
	return isRunning;
}

void Game::OnEnable()
{

	sendBuffer[0] = (char)ClientMessage::Join;
	client.SendingMsgs(std::ref(sendBuffer), 1);
}

void Game::UnpackingRecBuf(std::mutex& mtx)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));

	client.ReceivingMsgs(std::ref(recvBuffer));
	switch ((ServerMessage)recvBuffer[0])
	{
	case ServerMessage::Join_Result:
	{
		if (recvBuffer[1])
		{
			std::lock_guard<std::mutex> lm(mtx);
			//memcpy(&ownSlot, &recvBuffer[2], sizeof(ownSlot));
			client.ReadFromBuffer(ownSlot, recvBuffer, 2);

			std::cout << "server assigned us slot " << std::to_string(ownSlot) << std::endl;
		}
		else
		{
			std::cout << "server didn't let us in " << std::endl;
		}
	}
	break;
	case ServerMessage::State:
	{
		std::lock_guard<std::mutex> lm(mtx);
		int packetSize = client.ReceivingMsgs(std::ref(recvBuffer));
		int readIndex = 1;
		//int currentPlayerObject = 0;

		while (readIndex < packetSize)
		{
			unsigned short recvSlot;
			readIndex += client.ReadFromBuffer(recvSlot, recvBuffer, readIndex);
			if (std::find(slots.begin(), slots.end(), recvSlot) != slots.end())
			{
				if (recvSlot == ownSlot)
				{
					readIndex += client.ReadFromBuffer(localPlayer.x, recvBuffer, readIndex);

					readIndex += client.ReadFromBuffer(localPlayer.y, recvBuffer, readIndex);

					readIndex += client.ReadFromBuffer(localPlayer.facing, recvBuffer, readIndex);

					std::cout << "own player : " << "pos x : " << localPlayer.x << ", pos y: " << localPlayer.y << ", facing: " << localPlayer.facing << std::endl;
				}
				else
				{
					const auto it = std::find(slots.begin(), slots.end(), recvSlot);
					const unsigned short index = (unsigned short)std::distance(slots.begin(), it);

					readIndex += client.ReadFromBuffer(clientObjects.at(index).x, recvBuffer, readIndex);

					readIndex += client.ReadFromBuffer(clientObjects.at(index).y, recvBuffer, readIndex);

					readIndex += client.ReadFromBuffer(clientObjects.at(index).facing, recvBuffer, readIndex);

					std::cout << "ID : " << recvSlot << ", pos x : " << clientObjects.at(index).x << ", pos y: " << clientObjects.at(index).y << ", facing: " << clientObjects.at(index).facing << std::endl;
				}

			}
			else
			{
				if (recvSlot != 0xFFFF)
				{
					slots.push_back(recvSlot);
					clientObjects.push_back({});
				}
				else
				{
					const auto it = std::find(slots.begin(), slots.end(), recvSlot);
					const unsigned short index = (unsigned short)std::distance(slots.begin(), it);

					std::swap(slots.at(index), slots.back());
					slots.pop_back();

					std::swap(clientObjects.at(index), clientObjects.back());
					clientObjects.pop_back();

				}
			}
		}
	}
	break;
	}

}

void Game::PackingSendBuf(std::mutex& mtx)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
	if (ownSlot != 0xFFFF)
	{
		sendBuffer[0] = (char)ClientMessage::Input;
		int bytes_written = 1;

		bytes_written += client.WriteToBuffer(sendBuffer, bytes_written, ownSlot);
		//sendBuffer[1] = ownSlot;

		char input;
		if (kbd.isKeyPressed(sf::Keyboard::W))
		{
			input = 0x1;
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
		

		sendBuffer[3] = input;
		client.SendingMsgs(std::ref(sendBuffer), 4);
	}
	
	
	//sendBuffer[0] = (char)ClientMessage::Input;
	//client.ReadFromBuffer(ownSlot, recvBuffer, 1);
	//memcpy(&slot, &buffer[1], 2);
	
	
}

void Game::OnDisable()
{
	if (ownSlot != 0xFFFF)
	{
		sendBuffer[0] = (char)ClientMessage::Leave;
		client.WriteToBuffer(sendBuffer, 2, ownSlot);
	}
	client.SendingMsgs(std::ref(sendBuffer), 2);
	//slots.clear();
	//clientObjects.clear();
	isRunning = false;
}
