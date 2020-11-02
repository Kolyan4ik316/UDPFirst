#include "Game.h"

Game::Game()
{
	client.HintServer("127.0.0.1", 54000);
	player.setSize(sf::Vector2f(50.0f, 50.0f));
	player.setFillColor(sf::Color::Green);
}
void Game::Update(std::mutex& mtx, sf::RenderWindow& window)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const float dt = ft.Mark();
	std::lock_guard<std::mutex> lm(mtx);
	
	window.clear();
	
	

	player.setPosition(sf::Vector2f(localPlayer.x, localPlayer.y));
	window.draw(player);
	userTimeOut += dt;
	//std::cout << userTimeOut << std::endl;
	
	for (auto other = otherPlayers.begin(); other != otherPlayers.end(); other++)
	{
		const unsigned short index0 = (unsigned short)std::distance(otherPlayers.begin(), other);
		for (auto otherClient = clientObjects.begin(); otherClient != clientObjects.end(); otherClient++)
		{
			const unsigned short index1 = (unsigned short)std::distance(clientObjects.begin(), otherClient);
			if (index0 == index1)
			{
				time_since_heard_from_clients.at(index0) += dt;

				

				otherPlayers.at(index1).setPosition(sf::Vector2f(clientObjects.at(index0).x, clientObjects.at(index0).y));
				window.draw(otherPlayers.at(index0));
			}
		}
	}
	for (unsigned short index = 0; index < time_since_heard_from_clients.size(); index++)
	{
		//std::cout << slots.at(index) << " - user have time out: " << time_since_heard_from_clients.at(index) << std::endl;
		if (time_since_heard_from_clients.at(index) > timeOut)
		{

			std::swap(slots.at(index), slots.back());
			slots.pop_back();

			std::swap(clientObjects.at(index), clientObjects.back());
			clientObjects.pop_back();

			std::swap(otherPlayers.at(index), otherPlayers.back());
			otherPlayers.pop_back();

			std::swap(time_since_heard_from_clients.at(index), time_since_heard_from_clients.back());
			time_since_heard_from_clients.pop_back();
		}
	}
	
	
	
	window.display();
}

bool Game::IsRunning() const
{
	return isRunning;
}

void Game::OnEnable(std::mutex& mtx)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
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
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			std::lock_guard<std::mutex> lm(mtx);
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
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::lock_guard<std::mutex> lm(mtx);
		int packetSize = client.ReceivingMsgs(std::ref(recvBuffer));
		int readIndex = 1;
		
		while (readIndex < packetSize)
		{
			unsigned short recvSlot;
			readIndex += client.ReadFromBuffer(recvSlot, recvBuffer, readIndex);
			if ((std::find(slots.begin(), slots.end(), recvSlot) != slots.end()) && recvSlot != ownSlot)
			{
					
				const auto it = std::find(slots.begin(), slots.end(), recvSlot);
				const unsigned short index = (unsigned short)std::distance(slots.begin(), it);

				readIndex += client.ReadFromBuffer(clientObjects.at(index).x, recvBuffer, readIndex);

				readIndex += client.ReadFromBuffer(clientObjects.at(index).y, recvBuffer, readIndex);

				readIndex += client.ReadFromBuffer(time_since_heard_from_clients.at(index), recvBuffer, readIndex);
				//std::cout << "ID : " << recvSlot << ", pos x : " << clientObjects.at(index).x << ", pos y: " << clientObjects.at(index).y << std::endl;
						

			}
			else if(!(std::find(slots.begin(), slots.end(), recvSlot) != slots.end()) && recvSlot != ownSlot)
			{
				if (recvSlot != 0xFFFF)
				{
					float recivedX;
					float recivedY;
					float time;
					readIndex += client.ReadFromBuffer(recivedX, recvBuffer, readIndex);
					readIndex += client.ReadFromBuffer(recivedY, recvBuffer, readIndex);
					readIndex += client.ReadFromBuffer(time, recvBuffer, readIndex);
					slots.push_back(recvSlot);
					
					clientObjects.push_back(PlayerState{ recivedX, recivedY });
					otherPlayers.push_back(sf::RectangleShape(sf::Vector2f(50.0f, 50.0f)));
					otherPlayers.back().setFillColor(sf::Color::Red);
					time_since_heard_from_clients.push_back(time);
				}
				else
				{
					const auto it = std::find(slots.begin(), slots.end(), recvSlot);
					const unsigned short index = (unsigned short)std::distance(slots.begin(), it);

					std::swap(slots.at(index), slots.back());
					slots.pop_back();

					std::swap(clientObjects.at(index), clientObjects.back());
					clientObjects.pop_back();

					std::swap(otherPlayers.at(index), otherPlayers.back());
					otherPlayers.pop_back();

					std::swap(time_since_heard_from_clients.at(index), time_since_heard_from_clients.back());
					time_since_heard_from_clients.pop_back();

				}
			}
			else
			{
				if (recvSlot == ownSlot)
				{

					readIndex += client.ReadFromBuffer(localPlayer.x, recvBuffer, readIndex);

					readIndex += client.ReadFromBuffer(localPlayer.y, recvBuffer, readIndex);
					
					readIndex += client.ReadFromBuffer(userTimeOut, recvBuffer, readIndex);

					

					//std::cout << "own player : " << "pos x : " << localPlayer.x << ", pos y: " << localPlayer.y << std::endl;
				}
			}
		}
	}
	break;
	}

}

void Game::PackingSendBuf(std::mutex& mtx, bool& focused)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
	
	sendBuffer[0] = (char)ClientMessage::Input;

	int bytes_written = 1;
	bytes_written += client.WriteToBuffer(sendBuffer, bytes_written, ownSlot);
	if (focused)
	{

		if (ownSlot != 0xFFFF)
		{

			//sendBuffer[1] = ownSlot;
			input = 0;

			if (kbd.isKeyPressed(sf::Keyboard::W))
			{
				input = 1;
			}
			if (kbd.isKeyPressed(sf::Keyboard::S))
			{
				input = 2;
			}
			if (kbd.isKeyPressed(sf::Keyboard::A))
			{
				input = 3;
			}
			if (kbd.isKeyPressed(sf::Keyboard::D))
			{
				input = 4;
			}

		}

	}
	sendBuffer[3] = input;
	if ((client.SendingMsgs(std::ref(sendBuffer), 4) == 4))
	{
		userTimeOut = 0;
	}


	//sendBuffer[0] = (char)ClientMessage::Input;
	//client.ReadFromBuffer(ownSlot, recvBuffer, 1);
	//memcpy(&slot, &buffer[1], 2);
	
}

void Game::OnDisable(std::mutex& mtx)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
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
