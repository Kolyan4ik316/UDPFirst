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
		std::thread receiver([&]()
			{
				while (true)
				{
					mtx.lock();
					game.UnpackingRecBuf();
					mtx.unlock();
				}
			});
		while (game.IsRunning())
		{
			game.Update();
			game.PackingSendBuf();
			//game.RecivedFromClient();
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