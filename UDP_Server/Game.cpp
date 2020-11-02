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
		
		for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
		{
			const unsigned short i = (unsigned short)std::distance(clientAttr.begin(), it);
			if (clientAttr.at(i).ipPort.address)
			{
				PlayerState state = { 0.0f, 0.0f };

				if (clientAttr.at(i).input.up)
				{
					state.y -= acceleration * dt;
					state.x = 0;
				}
				if (clientAttr.at(i).input.down)
				{
					state.y += acceleration * dt;
					state.x = 0;
				}
				if (clientAttr.at(i).input.left)
				{
					state.x -= acceleration * dt;
					state.y = 0;
				}
				if (clientAttr.at(i).input.right)
				{
					state.x += acceleration * dt;
					state.y = 0;
				}
				if (clientAttr.at(i).input.empty)
				{
					state.x = 0;
					state.y = 0;
				}

				clientAttr.at(i).objects.x += state.x;
				clientAttr.at(i).objects.y += state.y;

				clientAttr.at(i).time_since_heard_from_client += dt;
				
			}
		}
		for (auto& e : clientAttr)
		{
			if (e.time_since_heard_from_client > clientTimeOut)
			{
				std::swap(e, clientAttr.back());
				clientAttr.pop_back();
			}
		}
		
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Udpating error" << oor.what() << std::endl;
		unsigned short slot;
		server.ReadFromBuffer(slot, recvBuffer, 1);
		for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
		{
			if (it->id == slot)
			{
				const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
				std::swap(clientAttr.at(index), clientAttr.back());
				clientAttr.pop_back();
			}
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
			
			//const auto ide = std::find(clientAttr.begin()->id, clientAttr.end()->id, slot);
			//std::vector<ClientAttributes>::iterator it;
			for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
			{
				if (it->id == slot)
				{
					const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
					std::swap(clientAttr.at(index), clientAttr.back());
					clientAttr.pop_back();
				}
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
		
			//slots.push_back(fromEndpoint.port);
			clientAttr.push_back(ClientAttributes{ fromEndpoint.port,fromEndpoint, {}, {}, 0.0f });

			sendBuffer[0] = (char)ServerMessage::Join_Result;
			if (clientAttr.back().id != unsigned short(-1))
			{
				sendBuffer[1] = 1;
				std::cout << "client will be assigned to slot " << clientAttr.back().id << std::endl;
				server.WriteToBuffer(sendBuffer, 2, clientAttr.back().id);
				server.SendingMsgs(sendBuffer, 4, from);
				if (server.IsSended())
				{
					if (fromEndpoint.address)
					{
						/*clientEndpoints.back() = fromEndpoint;
						time_since_heard_from_clients.back() = 0.0f;
						clientObjects.back() = {};
						clientInputs.back() = {};*/
						
						clientAttr.back().ipPort = fromEndpoint;
						clientAttr.back().time_since_heard_from_client = 0.0f;
						clientAttr.back().objects = {};
						clientAttr.back().input = {};
					}
				
				}
			}
			else
			{
				//std::lock_guard<std::mutex> lm(mtx);
				sendBuffer[1] = 0;
				server.SendingMsgs(sendBuffer, 2, from);

				std::cout << "Can't let client in " << fromEndpoint.address << ": " << fromEndpoint.port << std::endl;
			
			
				clientAttr.pop_back();
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
			
			//const auto ide = std::find(clientAttr.begin()->id, clientAttr.end()->id, slot);
			//std::vector<ClientAttributes>::iterator it;
			//unsigned short index = 0;
			for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
			{
				if (it->id == slot)
				{
					const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
					std::swap(clientAttr.at(index), clientAttr.back());
					clientAttr.pop_back();
				}
			}
				
			
			
		}
		catch (const std::out_of_range& oor)
		{
			std::cout << "Leave message error"<< oor.what() << std::endl;
			unsigned short slot;
			server.ReadFromBuffer(slot, recvBuffer, 1);
			
			for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
			{
				if (it->id == slot)
				{
					const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
					std::swap(clientAttr.at(index), clientAttr.back());
					clientAttr.pop_back();
				}
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
			
			for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
			{
				if (it->id == slot)
				{
					const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
					if (clientAttr.at(index).ipPort == fromEndpoint)
					{
						char input = recvBuffer[3];

						clientAttr.at(index).input.up = input == 1;
						clientAttr.at(index).input.down = input == 2;
						clientAttr.at(index).input.left = input == 3;
						clientAttr.at(index).input.right = input == 4;
						clientAttr.at(index).input.empty = input == 0;

						clientAttr.at(index).time_since_heard_from_client = 0.0f;

					}
				}
			}

			
			
			
		}
		catch (const std::out_of_range& oor)
		{
			std::cout << "Input message error" << oor.what() << std::endl;
			unsigned short slot;
			server.ReadFromBuffer(slot, recvBuffer, 1);
			
			for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
			{
				if (it->id == slot)
				{
					const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
					std::swap(clientAttr.at(index), clientAttr.back());
					clientAttr.pop_back();
				}
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
		
		for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
		{
			const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
			errorSlot = index;
			
			if (clientAttr.at(index).ipPort.address)
			{
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->id);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientAttr.at(index).objects.x);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientAttr.at(index).objects.y);
			}
		}
		
	}
	catch (const std::out_of_range& oor)
	{
		
		std::cout << "Packing message error" << oor.what() << std::endl;
		//std::lock_guard<std::mutex> lm(mtx);
		std::swap(clientAttr.at(errorSlot), clientAttr.back());
		clientAttr.pop_back();

		ZeroMemory(sendBuffer, 1024);
		
	}
	
		
	

	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(54000);
	int to_length = sizeof(to);
	
	
	
	
	try
	{
		
		for (auto it = clientAttr.begin(); it != clientAttr.end(); ++it)
		{
			const unsigned short index = (unsigned short)std::distance(clientAttr.begin(), it);
			errorSlot = index;
			if (clientAttr.at(index).ipPort.address)
			{
				to.sin_addr.S_un.S_addr = clientAttr.at(index).ipPort.address;
				to.sin_port = clientAttr.at(index).ipPort.port;
				
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
		std::swap(clientAttr.at(errorSlot), clientAttr.back());
		clientAttr.pop_back();

		to = {};

		ZeroMemory(sendBuffer, 1024);
	}
	
}
