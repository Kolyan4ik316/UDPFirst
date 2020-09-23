#include <iostream>
#include <thread>
#include <mutex>
#include "Server.h"
#include "Game.h"



int main()
{
	try
	{
		Game game;
		std::mutex mtx;
		std::thread receiver([&]() {
			while (true)
			{
				mtx.lock();
				game.UnpackingRecBuf();
				mtx.unlock();
				//std::cout << game.RecivedFromClient() << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			});
		while (true)
		{
			game.Update();
			game.PackingSendBuf();
			
		}
		receiver.detach();
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