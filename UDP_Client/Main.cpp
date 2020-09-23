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
		std::thread receiver([&]() {
			while (true)
			{
				mtx.lock();
				game.UnpackingRecBuf();
				mtx.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				std::cout << game.RecivedFromServer() << std::endl;
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