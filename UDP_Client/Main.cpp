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
		game.Run();
		
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