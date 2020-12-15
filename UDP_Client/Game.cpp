#include "Game.h"

void Game::InitWindow()
{
	std::ifstream ifs("config.ini");
	sf::VideoMode windowConfig(770, 650);
	std::string windowName = "Battle city online";
	unsigned int frameRateLimit = 60;
	bool vsync = false;
	serverAddress = "127.0.0.1";
	serverPort = 54000;

	if (ifs.is_open())
	{
		std::getline(ifs, windowName);
		ifs >> windowConfig.width >> windowConfig.height;
		ifs >> frameRateLimit;
		ifs >> vsync;
		ifs >> serverAddress;
		ifs >> serverPort;
	}
	ifs.close();
	window = std::make_shared<sf::RenderWindow>(windowConfig, windowName, sf::Style::Close);
	window->setFramerateLimit(frameRateLimit);
	window->setVerticalSyncEnabled(vsync);
	client = std::make_shared<Client>(serverAddress, serverPort);
}

void Game::InitStates()
{
	states.push(std::make_unique<GameState>(window, client));
}

void Game::QuitFromApplication()
{
	//OnDisable();
}

Game::Game()
{
	InitWindow();
	//client->HintServer(serverAddress, serverPort);
	InitStates();
	
}
void Game::Update()
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	dt = ft.Mark();
	UpdateSFMLEvents();
	

	if (!states.empty())
	{
		states.top()->FocusedState(focused);
		states.top()->Update(dt);

		if (states.top()->GetQuit())
		{
			states.top()->EndState();
			states.pop();
		}
	}
	else
	{
		QuitFromApplication();
		window->close();
	}

}

void Game::Render()
{
	window->clear();

	if (!states.empty())
	{
		states.top()->Render();
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
		{
			//OnDisable();
			window->close();
		}
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
	//OnEnable();
	while (window->isOpen())
	{
		Update();
		Render();
	}
	
}

Game::~Game()
{
}
