#include <thread>
#include <mutex>

#include "Client.h"
#include "Game.h"


int main()
{
	try
	{
		Game game;
		std::mutex mtx;
		std::thread sender([&]() {
			while (true)
			{
				mtx.lock();
				game.UnpackingRecBuf();
				mtx.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				
			}

			});
		while (true)
		{
			mtx.lock();
			game.PackingSendBuf();
			mtx.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			//std::cout << game.RecivedFromServer() << std::endl;
			game.Update();
			
		}
		sender.detach();
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