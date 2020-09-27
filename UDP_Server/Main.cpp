#include <iostream>
#include <thread>
#include "Server.h"
#include "Game.h"



int main()
{
	try
	{
		Game game;
		while (game.IsRunning())
		{
			game.Update();
			//game.RecivedFromClient();
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