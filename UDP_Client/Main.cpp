#include <thread>
#include "Game.h"
#include "FrameTimer.h"


/*int WINAPI WinMain(HINSTANCE hInstance,	
	HINSTANCE hPrevInstance,	
	LPSTR lpCmdLine,
	int nCmdShow)*/
int main()
{
	try
	{
		Game game;
		game.Run();
		
	}
	catch (std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Exception was throwned!", MB_OK);
	}
	catch (...)
	{
		MessageBoxA(NULL, "Uknown reason", "Exception was throwned!", MB_OK);
	}
	return 0;
}