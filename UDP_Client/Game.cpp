#include "Game.h"

Game::Game()
{
	client.HintServer("127.0.0.1", 54000);
	player.SetGraphicsAttr(sf::Vector2f(50.0f, 50.0f), sf::Color::Green);
}
void Game::Update(std::mutex& mtx, sf::RenderWindow& window)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const float dt = ft.Mark();
	std::lock_guard<std::mutex> lm(mtx);
	
	window.clear();
	
	
	player.shape.setPosition(sf::Vector2f(player.objects.x, player.objects.y));
	window.draw(player.shape);
	player.time_since_heard_from_client += dt;
	//std::cout << userTimeOut << std::endl;
	

	for (auto it = otherPlayers.begin(); it != otherPlayers.end(); it++)
	{
		it->time_since_heard_from_client += dt;
		
		if (it->stage == ClientStage::Broadcast)
		{
			it->SetGraphicsAttr(sf::Vector2f(50.0f, 50.0f), sf::Color::Yellow);
		}
		else
		{
			it->SetGraphicsAttr(sf::Vector2f(50.0f, 50.0f), sf::Color::Red);
		}
		it->shape.setPosition(sf::Vector2f(it->objects.x, it->objects.y));
		window.draw(it->shape);
		
	}

	for (unsigned short index = 0; index < otherPlayers.size(); index++)
	{
		//std::cout << slots.at(index) << " - user have time out: " << time_since_heard_from_clients.at(index) << std::endl;
		if (otherPlayers.at(index).time_since_heard_from_client > timeOut || otherPlayers.at(index).stage == ClientStage::Leave)
		{
			std::swap(otherPlayers.at(index), otherPlayers.back());
			otherPlayers.pop_back();

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

			//std::cout << "server assigned us slot " << std::to_string(ownSlot) << std::endl;
		}
		else
		{
			throw(std::exception("server didn't let us in"));
			//std::cout << "server didn't let us in " << std::endl;
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
			auto it = std::find_if(otherPlayers.begin(), otherPlayers.end(), [&recvSlot](const ClientAttributes& currAttrib) { return currAttrib.slot == recvSlot; });
			
			if (it != otherPlayers.end())
			{
				const unsigned short index = (unsigned short)std::distance(otherPlayers.begin(), it);
				
				char curState;
				readIndex += client.ReadFromBuffer(curState, recvBuffer, readIndex);
				otherPlayers.at(index).stage = ClientStage(curState);

				readIndex += client.ReadFromBuffer(otherPlayers.at(index).objects.x, recvBuffer, readIndex);

				readIndex += client.ReadFromBuffer(otherPlayers.at(index).objects.y, recvBuffer, readIndex);

				readIndex += client.ReadFromBuffer(otherPlayers.at(index).time_since_heard_from_client, recvBuffer, readIndex);

				
				
				//std::cout << "ID : " << recvSlot << ", pos x : " << clientObjects.at(index).x << ", pos y: " << clientObjects.at(index).y << std::endl;
						

			}
			else if(recvSlot != ownSlot)
			{
				
				
				float recivedX;
				float recivedY;
				float time;
				char curState;
				readIndex += client.ReadFromBuffer(recivedX, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(recivedY, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(time, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(curState, recvBuffer, readIndex);
				ClientStage currState = ClientStage(curState);
				PlayerState tempObj = PlayerState({ recivedX, recivedY });
				ClientAttributes tempAttributes(recvSlot, currState, tempObj, time);
				otherPlayers.push_back(tempAttributes);
			}
			else
			{
				if (recvSlot == ownSlot)
				{
					char curState;
					readIndex += client.ReadFromBuffer(curState, recvBuffer, readIndex);
					player.stage = ClientStage(curState);

					readIndex += client.ReadFromBuffer(player.objects.x, recvBuffer, readIndex);

					readIndex += client.ReadFromBuffer(player.objects.y, recvBuffer, readIndex);
					
					readIndex += client.ReadFromBuffer(player.time_since_heard_from_client, recvBuffer, readIndex);

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
		player.time_since_heard_from_client = 0;
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
