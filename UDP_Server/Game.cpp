#include "Game.h"
#include <algorithm>


Game::Game()
{
	// Binding our UDP server to port
	server.Bind(54000);
}
void Game::Update(std::mutex& mtx)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const float dt = ft.Mark();
	
	std::lock_guard<std::mutex> lm(mtx);
	
	try
	{
		// Looping our players objects
		for (auto it = clientAttr.begin(); it != clientAttr.end(); it++)
		{
			if (it->second.ipPort.address)
			{
				PlayerState state = { 0.0f, 0.0f };
				if (it->second.input.up)
				{
					state.y -= acceleration * dt;
					state.x = 0;
					it->second.direction = PlayerDirection::Up;
				}
				if (it->second.input.down)
				{
					state.y += acceleration * dt;
					state.x = 0;
					it->second.direction = PlayerDirection::Down;
				}
				if (it->second.input.left)
				{
					state.x -= acceleration * dt;
					state.y = 0;
					it->second.direction = PlayerDirection::Left;
				}
				if (it->second.input.right)
				{
					state.x += acceleration * dt;
					state.y = 0;
					it->second.direction = PlayerDirection::Right;
				}
				if (it->second.input.empty)
				{
					state.x = 0;
					state.y = 0;
				}

				it->second.objects.x += state.x;
				it->second.objects.y += state.y;

				it->second.time_since_heard_from_client += dt;

			}
			
		}
		// Deleting if someone of player didn't sending any message over timeout
		eraseIf(clientAttr, [](const std::pair<unsigned short, ClientAttributes>& mapy)
			{
				return mapy.second.time_since_heard_from_client > clientTimeOut;
			});
		
		
	}
	catch (const std::out_of_range& oor)
	{
		// If we are looping over range
		std::cout << "Udpating error" << oor.what() << std::endl;
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
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server.GetSocket(), SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);
		
		
		if (WSAGetLastError() == 0)
		{
			// Just passing if we catch not full message
			fromEndpoint = {};
			ZeroMemory(recvBuffer, 1024);
		}
		else
		{
			// Other error messages
			std::cout << "Error with receiving data " << WSAGetLastError() << std::endl;
		}
		
	}
	
	switch ((ClientMessage)recvBuffer[0])
	{

	case ClientMessage::Join:
	{

		std::lock_guard<std::mutex> lm(mtx);
		//unsigned short slot = unsigned short(-1);
		
		
		if (fromEndpoint.address)
		{
			std::cout << "Client_Message::Join from " << fromEndpoint.address << ": " << fromEndpoint.port << std::endl;
			const unsigned short slot = fromEndpoint.port;
			//server.ReadFromBuffer(slot, recvBuffer, 1);
			clientAttr.insert(std::pair<unsigned short, ClientAttributes>({ slot }, ClientAttributes{ fromEndpoint, ClientStage::Broadcast, {}, {},PlayerDirection::Up, 0.0f }));
			auto it = clientAttr.find(slot);
			// To prevent end of our container
			//it--;
			
			// Assigning join result to send buffer
			sendBuffer[0] = (char)ServerMessage::Join_Result;
			// 1 of [1] if it was succesfull
			sendBuffer[1] = 1;
			std::cout << "client will be assigned to slot " << it->first << std::endl;
			// Writing result to buffer and sening this message to client itself
			server.WriteToBuffer(sendBuffer, 2, it->first);
			server.SendingMsgs(sendBuffer, 4, from);
				
			if (!(server.IsSended()))
			{
				// If failed with sending didn't assigning
				// 0 of [1] if it failed
				sendBuffer[1] = 0;
				server.SendingMsgs(sendBuffer, 2, from);

				std::cout << "Can't let client in " << fromEndpoint.address << ": " << fromEndpoint.port << std::endl;

				// Deleting new object
				it->second.state = ClientStage::Leave;
				clientAttr.erase(it);
				fromEndpoint = {};

			}
			
			
			
			
		}
	}
	break;
	case ClientMessage::Leave:
	{
		//std::lock_guard<std::mutex> lm(mtx);
		//mtx.lock();
		std::cout << "Client_Message::Leave from " << fromEndpoint.address <<": "<<fromEndpoint.port << std::endl;
		const unsigned short slot = fromEndpoint.port;
		//server.ReadFromBuffer(slot, recvBuffer, 1);
		//mtx.unlock();
		try
		{
			mtx.lock();
			auto it = clientAttr.find(slot);
			mtx.unlock();
			if (it != clientAttr.end())
			{
				mtx.lock();
				it->second.state = ClientStage::Leave;
				mtx.unlock();
				PackingSendBuf(mtx);
				mtx.lock();
				clientAttr.erase(it);
				mtx.unlock();
			}
		}
		catch (const std::out_of_range& oor)
		{
			std::cout << "Leave message error"<< oor.what() << std::endl;
			
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
			auto it = clientAttr.find(slot);
			if (it != clientAttr.end())
			{
				char input = recvBuffer[3];
				it->second.input.up = input == 1;
				it->second.input.down = input == 2;
				it->second.input.left = input == 3;
				it->second.input.right = input == 4;
				it->second.input.empty = input == 0;

				it->second.time_since_heard_from_client = 0.0f;

			}
			
		}
		catch (const std::out_of_range& oor)
		{
			std::cout << "Input message error" << oor.what() << std::endl;
		}
		
		
	}
	break;
	}
}

void Game::PackingSendBuf(std::mutex& mtx)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	std::lock_guard<std::mutex> lm(mtx);
	sendBuffer[0] = (char)ServerMessage::State;
	int bytesWriten = 1;
	
	
	try
	{
		
		for (auto it = clientAttr.begin(); it != clientAttr.end(); it++)
		{
			if (it->second.ipPort.address)
			{
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->first);

				char currState = char(it->second.state);
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, currState);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->second.objects.x);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->second.objects.y);

				char currDir= char(it->second.direction);
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, currDir);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->second.time_since_heard_from_client);
				

			}
			
		}
		
	}
	catch (const std::out_of_range& oor)
	{
		
		std::cout << "Packing message error" << oor.what() << std::endl;
		ZeroMemory(sendBuffer, 1024);
		
	}
	
		
	

	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(54000);
	int to_length = sizeof(to);
	
	
	
	
	try
	{
		
		for (auto it = clientAttr.begin(); it != clientAttr.end(); it++)
		{
			if (it->second.ipPort.address)
			{
				to.sin_addr.S_un.S_addr = it->second.ipPort.address;
				to.sin_port = it->second.ipPort.port;

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

		to = {};

		ZeroMemory(sendBuffer, 1024);
	}
	
}
