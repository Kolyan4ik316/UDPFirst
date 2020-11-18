#include <iostream>
#include <thread>
#include "Server.h"
#include "Game.h"



int main()
{
	try
	{
		Game game;
		std::mutex mtx;
		std::cout << "Server start work" << std::endl;
		std::vector<std::thread> threads;
		for (size_t i = 0; i < std::thread::hardware_concurrency(); i++)
		{
			threads.emplace_back([&]() 
				{
					while (game.IsRunning())
					{
						game.UnpackingRecBuf(std::ref(mtx));
						game.Update(std::ref(mtx));
						game.PackingSendBuf(std::ref(mtx));
					}
					
				});
		}
		std::cout << "You have " << threads.size() << " threads!" << std::endl;
		for (auto& e : threads)
		{
			if (e.joinable())
			{
				e.join();
			}
		}
		
		
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Something went wrong!" << std::endl;
	}
	system("pause");
	return 0;
}