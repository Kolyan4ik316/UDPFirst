#include <thread>
#include "Game.h"
#include "FrameTimer.h"


int WINAPI WinMain(HINSTANCE hInstance,	
	HINSTANCE hPrevInstance,	
	LPSTR lpCmdLine,
	int nCmdShow)
{
	try
	{
		Game game;
		std::mutex mtx;
		game.OnEnable(std::ref(mtx));
		//FrameTimer ft;
		//std::cout << "Client start work" << std::endl;
		sf::RenderWindow window(sf::VideoMode(770, 650), "Battle city online", sf::Style::Close);
		std::thread receiver([&]()
			{
				while (game.IsRunning())
				{
					game.UnpackingRecBuf(std::ref(mtx));
				}
				/*if (!game.IsRunning())
				{
					window.close();
				}*/
				
			});
		
		bool focused = true;
		while (window.isOpen())
		{
			//const float dt = ft.Mark();
			sf::Event evnt;
			while (window.pollEvent(evnt))
			{
				switch (evnt.type)
				{
				case sf::Event::Closed:
					if (game.IsRunning())
					{
						game.OnDisable(std::ref(mtx));
					}
					window.close();
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
				case sf::Event::KeyPressed:
				{
					if (focused)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
						{
							game.OnDisable(std::ref(mtx));
							window.close();
						}
					}
				}
				break;
				}
				
			}
			
			game.Update(std::ref(mtx), window);
			
			game.PackingSendBuf(std::ref(mtx), focused);
		
			
		}
		receiver.join();
		//game.OnDisable();
		
	}
	catch (std::exception& e)
	{
		//std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		//std::cout << "Something went wrong!" << std::endl;
	}
	return 0;
}