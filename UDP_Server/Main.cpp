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
		std::thread receiver([&]()
			{
				while (game.IsRunning())
				{
					game.UnpackingRecBuf(std::ref(mtx));
					//std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
		while (game.IsRunning())
		{
			game.Update(std::ref(mtx));
			game.PackingSendBuf(std::ref(mtx));
			
			//game.RecivedFromClient();
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
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