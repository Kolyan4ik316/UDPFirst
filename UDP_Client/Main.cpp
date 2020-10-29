#include <thread>

#include "Client.h"
#include "Game.h"


int main()
{
	try
	{
		Game game;
		std::mutex mtx;
		game.OnEnable();
		std::cout << "Client start work" << std::endl;
		std::thread receiver([&]()
			{
				while (game.IsRunning())
				{
					game.UnpackingRecBuf(std::ref(mtx));
				}
				
			});
		
		while (game.IsRunning())
		{
			game.Update(std::ref(mtx));
			game.PackingSendBuf(std::ref(mtx));
			
			
			//std::cout << game.RecivedFromServer() << std::endl;
			
		}
		receiver.join();
		//game.OnDisable();
		
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Something went wrong!" << std::endl;
	}
	return 0;
}