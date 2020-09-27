#include <thread>
#include <mutex>

#include "Client.h"
#include "Game.h"


int main()
{
	try
	{
		Game game;
		game.OnEnable();
		
		while (game.IsRunning())
		{
			game.Update();
			
			game.UnpackingRecBuf();
			
			//std::cout << game.RecivedFromServer() << std::endl;
			
		}
		game.OnDisable();
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