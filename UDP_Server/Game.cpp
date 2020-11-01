#include "Game.h"
#include <algorithm>
#include <memory>


#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)


Game::Game()
{
	server.Bind(54000);
	
}
std::string Game::RecivedFromClient() const
{
	return std::string(clientIP + " " + clientPort + ": " + std::string(recvBuffer) + " bytes recived = ");
}
void Game::Update(std::mutex& mtx)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const float dt = ft.Mark();
	
	std::lock_guard<std::mutex> lm(mtx);
	
	try
	{
		
		for (auto it = slots.begin(); it != slots.end(); ++it)
		{
			const unsigned short i = (unsigned short)std::distance(slots.begin(), it);
			if (clientEndpoints.at(i).address)
			{
				PlayerState state = { 0.0f, 0.0f };

				if (clientInputs.at(i).up)
				{
					state.y -= acceleration * dt;
					state.x = 0;
				}
				if (clientInputs.at(i).down)
				{
					state.y += acceleration * dt;
					state.x = 0;
				}
				if (clientInputs.at(i).left)
				{
					state.x -= acceleration * dt;
					state.y = 0;
				}
				if (clientInputs.at(i).right)
				{
					state.x += acceleration * dt;
					state.y = 0;
				}
				if (clientInputs.at(i).empty)
				{
					state.x = 0;
					state.y = 0;
				}
				
				
				clientObjects.at(i).x += state.x;
				clientObjects.at(i).y += state.y;

				time_since_heard_from_clients.at(i) += dt;
				if (time_since_heard_from_clients.at(i) > clientTimeOut)
				{
					std::swap(clientEndpoints.at(i), clientEndpoints.back());
					clientEndpoints.pop_back();

					std::swap(time_since_heard_from_clients.at(i), time_since_heard_from_clients.back());
					time_since_heard_from_clients.pop_back();

					std::swap(clientObjects.at(i), clientObjects.back());
					clientObjects.pop_back();

					std::swap(clientInputs.at(i), clientInputs.back());
					clientInputs.pop_back();
				}
			}
		}
		
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Udpating error" << oor.what() << std::endl;
		unsigned short slot;
		server.ReadFromBuffer(slot, recvBuffer, 1);
		if (std::find(slots.begin(), slots.end(), slot) != slots.end())
		{
			const auto it = std::find(slots.begin(), slots.end(), slot);
			const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
			std::swap(slots.at(index), slots.back());
			slots.pop_back();
		}

	}
	
}
bool Game::IsRunning() const
{
	return isRunning;
}

void Game::UnpackingRecBuf(std::mutex& mtx)
{
	
	sockaddr_in from;
	IP_Endpoint fromEndpoint;
	int bytesReceived;
	bytesReceived = server.ReceivingMsgs(recvBuffer, from);
	fromEndpoint.address = from.sin_addr.S_un.S_addr;
	fromEndpoint.port = from.sin_port;
	if (bytesReceived == SOCKET_ERROR)
	{
		//IP_Endpoint fromEndpoint = {};
		//sockaddr_in from = {};
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server.GetSocket(), SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);
		
		
		if (WSAGetLastError() == 0)
		{
			unsigned short slot;
			server.ReadFromBuffer(slot, recvBuffer, 1);
			if (std::find(slots.begin(), slots.end(), slot) != slots.end())
			{
				const auto it = std::find(slots.begin(), slots.end(), slot);
				const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
				std::swap(slots.at(index), slots.back());
				slots.pop_back();
			}
			fromEndpoint = {};
			ZeroMemory(recvBuffer, 1024);
		}
		else
		{
			std::cout << "Error with receiving data " << WSAGetLastError() << std::endl;
		}
		
	}
	
	switch ((ClientMessage)recvBuffer[0])
	{

	case ClientMessage::Join:
	{

		std::lock_guard<std::mutex> lm(mtx);
		unsigned short slot = unsigned short(-1);
		
		
		if (fromEndpoint.address)
		{
			std::cout << "Client_Message::Join from " << fromEndpoint.address << ": " << fromEndpoint.port << std::endl;
			clientEndpoints.push_back({});
			time_since_heard_from_clients.push_back(0.0f);
			clientObjects.push_back({});
			clientInputs.push_back({});
			slots.push_back(fromEndpoint.port);
		
		
		

			sendBuffer[0] = (char)ServerMessage::Join_Result;
			if (slots.back() != unsigned short(-1))
			{
				sendBuffer[1] = 1;
				std::cout << "client will be assigned to slot " << slots.back() << std::endl;
				server.WriteToBuffer(sendBuffer, 2, slots.back());
				server.SendingMsgs(sendBuffer, 4, from);
				if (server.IsSended())
				{
					if (fromEndpoint.address)
					{
						clientEndpoints.back() = fromEndpoint;
						time_since_heard_from_clients.back() = 0.0f;
						clientObjects.back() = {};
						clientInputs.back() = {};
					}
				
				}
			}
			else
			{
				//std::lock_guard<std::mutex> lm(mtx);
				sendBuffer[1] = 0;
				server.SendingMsgs(sendBuffer, 2, from);

				std::cout << "Can't let client in " << fromEndpoint.address << ": " << fromEndpoint.port << std::endl;
			
			
				slots.pop_back();
			
			
				clientEndpoints.pop_back();

			
				time_since_heard_from_clients.pop_back();

			
				clientObjects.pop_back();

			
				clientInputs.pop_back();

				fromEndpoint = {};
			
			}
		}
	}
	break;
	case ClientMessage::Leave:
	{
		std::lock_guard<std::mutex> lm(mtx);
		std::cout << "Client_Message::Leave from " << fromEndpoint.address <<": "<<fromEndpoint.port << std::endl;
		unsigned short slot;
		server.ReadFromBuffer(slot, recvBuffer, 1);
		try
		{
			//ZeroMemory(recvBuffer, 1024);
			if (std::find(slots.begin(), slots.end(), slot) != slots.end())
			{
				const auto it = std::find(slots.begin(), slots.end(), slot);
				const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
				
				if (clientEndpoints.at(index) == fromEndpoint)
				{
					
					std::swap(slots.at(index), slots.back());
					slots.pop_back();

					std::swap(clientEndpoints.at(index), clientEndpoints.back());
					clientEndpoints.pop_back();

					std::swap(time_since_heard_from_clients.at(index), time_since_heard_from_clients.back());
					time_since_heard_from_clients.pop_back();

					std::swap(clientObjects.at(index), clientObjects.back());
					clientObjects.pop_back();

					std::swap(clientInputs.at(index), clientInputs.back());
					clientInputs.pop_back();
					
				}
				
			}
			
		}
		catch (const std::out_of_range& oor)
		{
			std::cout << "Leave message error"<< oor.what() << std::endl;
			unsigned short slot;
			server.ReadFromBuffer(slot, recvBuffer, 1);
			if (std::find(slots.begin(), slots.end(), slot) != slots.end())
			{
				const auto it = std::find(slots.begin(), slots.end(), slot);
				const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
				std::swap(slots.at(index), slots.back());
				slots.pop_back();
			}
		}
	}
	break;
	case ClientMessage::Input:
	{
		std::lock_guard<std::mutex> lm(mtx);
		//std::cout << "Client_Message::Input from \n"<<fromEndpoint.address << std::endl;
		unsigned short slot;

		
		server.ReadFromBuffer(slot, recvBuffer, 1);
		
		try
		{
			if (std::find(slots.begin(), slots.end(), slot) != slots.end())
			{
				
				const auto it = std::find(slots.begin(), slots.end(), slot);
				const unsigned short index = (unsigned short)std::distance(slots.begin(), it);

				if (clientEndpoints.at(index) == fromEndpoint)
				{
					char input = recvBuffer[3];
					
					clientInputs.at(index).up = input == 1;
					clientInputs.at(index).down = input == 2;
					clientInputs.at(index).left = input == 3;
					clientInputs.at(index).right = input == 4;
					clientInputs.at(index).empty = input  == 0;

					time_since_heard_from_clients.at(index) = 0.0f;
					
				}
			}
			
		}
		catch (const std::out_of_range& oor)
		{
			std::cout << "Input message error" << oor.what() << std::endl;
			unsigned short slot;
			server.ReadFromBuffer(slot, recvBuffer, 1);
			if (std::find(slots.begin(), slots.end(), slot) != slots.end())
			{
				const auto it = std::find(slots.begin(), slots.end(), slot);
				const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
				std::swap(slots.at(index), slots.back());
				slots.pop_back();
			}
		}
		
		
	}
	break;
	}
}

void Game::PackingSendBuf(std::mutex& mtx)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
	sendBuffer[0] = (char)ServerMessage::State;
	int bytesWriten = 1;
	
	
	try
	{
		
		for (auto it = slots.begin(); it != slots.end(); ++it)
		{
			const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
			errorSlot = index;
			
			if (clientEndpoints.at(index).address)
			{
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, *it);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects.at(index).x);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects.at(index).y);
			}
		}
		
	}
	catch (const std::out_of_range& oor)
	{
		
		std::cout << "Packing message error" << oor.what() << std::endl;
		//std::lock_guard<std::mutex> lm(mtx);
		std::swap(slots.at(errorSlot), slots.back());
		slots.pop_back();

		ZeroMemory(sendBuffer, 1024);
		
	}
	
		
	

	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(54000);
	int to_length = sizeof(to);
	
	
	
	
	try
	{
		
		for (auto it = slots.begin(); it != slots.end(); ++it)
		{
			const unsigned short index = (unsigned short)std::distance(slots.begin(), it);
			errorSlot = index;
			if (clientEndpoints.at(index).address)
			{
				to.sin_addr.S_un.S_addr = clientEndpoints.at(index).address;
				to.sin_port = clientEndpoints.at(index).port;
				
				if (to.sin_addr.S_un.S_addr)
				{
					server.SendingMsgs(sendBuffer, bytesWriten, to);
				}
				
			}
		}
		
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Sending packed message error" << oor.what() << std::endl;
		std::swap(slots.at(errorSlot), slots.back());
		slots.pop_back();

		to = {};

		ZeroMemory(sendBuffer, 1024);
	}
	
}
