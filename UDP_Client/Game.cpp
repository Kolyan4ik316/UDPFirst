#include "Game.h"

void Game::InitWindow()
{
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(770, 650), "Battle city online", sf::Style::Close);
	//window->setKeyRepeatEnabled(false);
	window->setFramerateLimit(60);
}

Game::Game()
{
	client.HintServer("127.0.0.1", 54000);
	InitWindow();
	if (!font.loadFromFile("arial.ttf"))
	{
		throw(std::exception("Can't find your font!"));
	}
	player.text.setFont(font);
	player.text.setCharacterSize(30);
	player.text.setFillColor(sf::Color::Green);
	player.text.setStyle(sf::Text::Bold | sf::Text::Underlined);


	if (!texture.loadFromFile("battle_city_sprites.png", sf::IntRect(0, 0, 16, 16)))
	{
		throw(std::exception("Error with loading texture!"));
	}

	player.sprite.setTexture(texture);
	player.sprite.setOrigin(8, 8);
	player.sprite.scale(2.7f, 2.7f);

}
void Game::Update()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const float dt = ft.Mark();
	std::lock_guard<std::mutex> lm(mtx);
	player.text.setString(std::to_string(ownSlot));
	player.text.setPosition(sf::Vector2f(player.objects.x - 10.0f, player.objects.y - 10.0f));
	//window.draw(player.shape);
	player.Update(player.direction);
	player.sprite.setPosition(sf::Vector2f(player.objects.x, player.objects.y));


	player.time_since_heard_from_client += dt;
	//std::cout << userTimeOut << std::endl;


	for (auto it = otherPlayers.begin(); it != otherPlayers.end(); it++)
	{
		it->time_since_heard_from_client += dt;

		//it->shape.setPosition(sf::Vector2f(it->objects.x, it->objects.y));
		it->text.setPosition(sf::Vector2f(it->objects.x - 10.0f, it->objects.y - 10.0f));
		it->text.setString(std::to_string(it->slot));
		it->sprite.setPosition(sf::Vector2f(it->objects.x, it->objects.y));
		it->Update(it->direction);


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


}

void Game::Render()
{
	window->clear();
	window->draw(player.sprite);
	window->draw(player.text);

	for (auto it = otherPlayers.begin(); it != otherPlayers.end(); it++)
	{

		//window.draw(it->shape);
		window->draw(it->sprite);
		window->draw(it->text);
	}
	window->display();
}

void Game::UpdateSFMLEvents()
{
	while (window->pollEvent(sfEvent))
	{
		switch (sfEvent.type)
		{
		case sf::Event::Closed:
			if (IsRunning())
			{
				OnDisable();
			}
			window->close();
			break;
		case sf::Event::Resized:
			//std::cout << "Size of window : width : " << evnt.size.width << ", height : " << evnt.size.height << std::endl;
			break;
		case sf::Event::GainedFocus:
		{
			focused = true;
		}
		break;
		case sf::Event::LostFocus:
		{
			focused = false;
		}
		break;
		}

	}
}

void Game::Run()
{
	OnEnable();
	std::thread reciver([this]()
		{
			while (window->isOpen())
			{
				UnpackingRecBuf();
			}

		});
	while (window->isOpen())
	{
		UpdateSFMLEvents();
		Update();
		Render();
		PackingSendBuf();
	}
}

Game::~Game()
{
}

bool Game::IsRunning() const
{
	return isRunning;
}

void Game::OnEnable()
{
	std::lock_guard<std::mutex> lm(mtx);
	sendBuffer[0] = (char)ClientMessage::Join;
	client.SendingMsgs(std::ref(sendBuffer), 1);
}

void Game::UnpackingRecBuf()
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

				char curDir;
				readIndex += client.ReadFromBuffer(curDir, recvBuffer, readIndex);
				otherPlayers.at(index).direction = PlayerDirection(curDir);


				readIndex += client.ReadFromBuffer(otherPlayers.at(index).time_since_heard_from_client, recvBuffer, readIndex);



				//std::cout << "ID : " << recvSlot << ", pos x : " << clientObjects.at(index).x << ", pos y: " << clientObjects.at(index).y << std::endl;


			}
			else if (recvSlot != ownSlot)
			{


				float recivedX;
				float recivedY;
				float time;
				char curState;
				char curDir;
				readIndex += client.ReadFromBuffer(curState, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(recivedX, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(recivedY, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(curDir, recvBuffer, readIndex);
				readIndex += client.ReadFromBuffer(time, recvBuffer, readIndex);

				ClientStage currState = ClientStage(curState);
				PlayerState tempObj = PlayerState({ recivedX, recivedY });
				PlayerDirection tempDir = PlayerDirection(curDir);
				ClientAttributes tempAttributes(recvSlot, currState, tempObj, tempDir, time);
				otherPlayers.push_back(tempAttributes);
				otherPlayers.back().text.setFont(font);
				otherPlayers.back().text.setCharacterSize(30);
				otherPlayers.back().text.setFillColor(sf::Color::Red);
				otherPlayers.back().text.setStyle(sf::Text::Bold | sf::Text::Underlined);
				otherPlayers.back().sprite.setTexture(texture);
				otherPlayers.back().sprite.setOrigin(8, 8);
				otherPlayers.back().sprite.scale(2.7f, 2.7f);
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

					char curDir;
					readIndex += client.ReadFromBuffer(curDir, recvBuffer, readIndex);
					player.direction = PlayerDirection(curDir);

					readIndex += client.ReadFromBuffer(player.time_since_heard_from_client, recvBuffer, readIndex);

				}
			}
		}
	}
	break;
	}

}

void Game::PackingSendBuf()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);

	sendBuffer[0] = (char)ClientMessage::Input;

	int bytes_written = 1;
	bytes_written += client.WriteToBuffer(sendBuffer, bytes_written, ownSlot);
	input = 0;
	if (focused)
	{

		if (kbd.isKeyPressed(sf::Keyboard::Q))
		{
			OnDisable();

			window->close();
		}
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

		//kbd = {};
		//sfEvent = {};
	}
	sendBuffer[3] = input;
	if ((client.SendingMsgs(std::ref(sendBuffer), 4) == 4))
	{
		player.time_since_heard_from_client = 0;
	}

	//input = 0;
	//sendBuffer[0] = (char)ClientMessage::Input;
	//client.ReadFromBuffer(ownSlot, recvBuffer, 1);
	//memcpy(&slot, &buffer[1], 2);

}

void Game::OnDisable()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
