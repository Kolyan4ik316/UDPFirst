#include <iostream>
#include <thread>
#include "Server.h"
#include <string>
#include <mutex>


int main()
{
	try
	{
		Server server;
		server.Bind(54000);
		char recvBuf[1024];
		char sendBuf[1024];
		std::mutex mtx;
		std::string exitMsg;


		std::thread reciver([&]() 
			{
				while (true)
				{
					ZeroMemory(recvBuf, sizeof(recvBuf));
					mtx.lock();
					server.ReceivingMsgs(std::ref(recvBuf));
					std::cout << recvBuf << std::endl;
					exitMsg = std::string(recvBuf);
					mtx.unlock();
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				
			});
		bool isRunning = true;
		while (isRunning)
		{
			ZeroMemory(sendBuf, 1024);

			server.SendingMsgs(std::ref(sendBuf));
			
			if (exitMsg == "quit")
			{
				mtx.lock();
				isRunning = false;
				
				mtx.unlock();
			}
			exitMsg.clear();
		}
		reciver.detach();
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