#include "GameState.h"

GameState::GameState(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Client> client)
	: 
	State(window, client)
{
	player.LoadTexture("battle_city_sprites.png", sf::IntRect(0, 0, 16, 16));
	OnEnable();
	threads.emplace([this]() {while (!GetQuit())
	{
		UnpackMsg();
	}});
}

void GameState::Update(const float& dt)
{
	//std::cout << "Hello from game state!" << std::endl;
	//UpdateInput(dt);
	for (size_t i = 0; i < otherPlayers.size(); i++)
	{
		otherPlayers.at(i).Update(dt);
		if (otherPlayers.at(i).LeaveCondition())
		{
			std::swap(otherPlayers.at(i), otherPlayers.back());
			otherPlayers.pop_back();
		}
	}
	player.Update(dt);
	//UnpackMsg();
	UpdateInput(dt);
	PackMsg();
	
	
	
}

void GameState::Render(sf::RenderTarget* target)
{
	if (!target)
	{
		target = window.get();
	}
	for (size_t i = 0; i < otherPlayers.size(); i++)
	{
		otherPlayers.at(i).Render(target);
	}
	player.Render(target);
}

void GameState::UpdateInput(const float& dt)
{
	input = 0;
	if (IsFocused())
	{
		CheckForQuit();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			input = 3;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			input = 4;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			input = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			input = 2;
		}
	}
	
}

void GameState::UnpackMsg()
{
	client->ReceivingMsgs(std::ref(recvBuffer));
	std::lock_guard<std::mutex> lm(mtx);
	switch ((ServerMessage)recvBuffer[0])
	{
	case ServerMessage::Join_Result:
	{
		if (recvBuffer[1])
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			
			client->ReadFromBuffer(ownSlot, recvBuffer, 2);

			//std::cout << "server assigned us slot " << std::to_string(ownSlot) << std::endl;
		}
		else
		{
			//OnEnable();
			if (player.time_since_heard_from_client > 15.0f)
			{
				throw(std::exception("server didn't let us in"));
			}
			//std::cout << "server didn't let us in " << std::endl;
		}
	}
	break;
	case ServerMessage::State:
	{
		
		int packetSize = client->ReceivingMsgs(std::ref(recvBuffer));
		int readIndex = 1;

		while (readIndex < packetSize)
		{
			unsigned short recvSlot;

			readIndex += client->ReadFromBuffer(recvSlot, recvBuffer, readIndex);
			auto it = std::find_if(otherPlayers.begin(), otherPlayers.end(), [&recvSlot](const Tank& currAttrib) { return currAttrib.slot == recvSlot; });

			if (it != otherPlayers.end())
			{
				const unsigned short index = (unsigned short)std::distance(otherPlayers.begin(), it);

				char curState;
				readIndex += client->ReadFromBuffer(curState, recvBuffer, readIndex);
				otherPlayers.at(index).stage = ClientStage(curState);

				float x, y;
				readIndex += client->ReadFromBuffer(x, recvBuffer, readIndex);

				readIndex += client->ReadFromBuffer(y, recvBuffer, readIndex);
				otherPlayers.at(index).SetPosition(x, y);
				char curDir;
				readIndex += client->ReadFromBuffer(curDir, recvBuffer, readIndex);
				otherPlayers.at(index).SetDirection(PlayerDirection(curDir));


				readIndex += client->ReadFromBuffer(otherPlayers.at(index).time_since_heard_from_client, recvBuffer, readIndex);


			}
			else if (recvSlot != ownSlot)
			{


				float recivedX;
				float recivedY;
				float time;
				char curState;
				char curDir;
				readIndex += client->ReadFromBuffer(curState, recvBuffer, readIndex);
				readIndex += client->ReadFromBuffer(recivedX, recvBuffer, readIndex);
				readIndex += client->ReadFromBuffer(recivedY, recvBuffer, readIndex);
				readIndex += client->ReadFromBuffer(curDir, recvBuffer, readIndex);
				readIndex += client->ReadFromBuffer(time, recvBuffer, readIndex);

				ClientStage currState = ClientStage(curState);
				PlayerDirection tempDir = PlayerDirection(curDir);
				//ClientAttributes tempAttributes(recvSlot, currState, tempObj, tempDir, time);
				otherPlayers.push_back(recvSlot);
				otherPlayers.back().LoadTexture("battle_city_sprites.png", sf::IntRect(0, 0, 16, 16));
				otherPlayers.back().stage = ClientStage(curState);
				otherPlayers.back().SetPosition(recivedX, recivedY);
				otherPlayers.back().SetDirection(tempDir);
				otherPlayers.back().text.setFont(font);
				otherPlayers.back().text.setCharacterSize(30);
				otherPlayers.back().text.setFillColor(sf::Color::Red);
				otherPlayers.back().text.setStyle(sf::Text::Bold | sf::Text::Underlined);
			}
			else
			{
				if (recvSlot == ownSlot)
				{
					
					player.slot = ownSlot;
					char curState;
					readIndex += client->ReadFromBuffer(curState, recvBuffer, readIndex);
					player.stage = ClientStage(curState);
					float x, y;
					readIndex += client->ReadFromBuffer(x, recvBuffer, readIndex);

					readIndex += client->ReadFromBuffer(y, recvBuffer, readIndex);

					player.SetPosition(x, y);
					char curDir;
					readIndex += client->ReadFromBuffer(curDir, recvBuffer, readIndex);
					player.SetDirection(PlayerDirection(curDir));

					readIndex += client->ReadFromBuffer(player.time_since_heard_from_client, recvBuffer, readIndex);

					player.text.setFont(font);
					player.text.setCharacterSize(30);
					player.text.setFillColor(sf::Color::Green);
					player.text.setStyle(sf::Text::Bold | sf::Text::Underlined);

				}
			}
		}
	}
	break;
	}
}

void GameState::PackMsg()
{
	std::lock_guard<std::mutex> lm(mtx);
	sendBuffer[0] = (char)ClientMessage::Input;
	int bytes_written = 1;
	bytes_written += client->WriteToBuffer(sendBuffer, bytes_written, ownSlot);
	sendBuffer[3] = input;
	if ((client->SendingMsgs(std::ref(sendBuffer), 4) == 4))
	{
		player.time_since_heard_from_client = 0;
	}
}

void GameState::EndState()
{
}

void GameState::OnEnable()
{
	std::lock_guard<std::mutex> lm(mtx);
	sendBuffer[0] = (char)ClientMessage::Join;
	client->SendingMsgs(std::ref(sendBuffer), 1);
}

void GameState::OnDisable()
{
	std::lock_guard<std::mutex> lm(mtx);
	sendBuffer[0] = (char)ClientMessage::Leave;
	client->WriteToBuffer(sendBuffer, 2, ownSlot);
	
	client->SendingMsgs(std::ref(sendBuffer), 2);
}

GameState::~GameState()
{
	for (size_t i = 0; i < threads.size(); i++)
	{
		if (threads.top().joinable())
		{
			threads.top().detach();
			threads.pop();
		}
	}
}
