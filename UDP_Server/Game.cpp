#include "Game.h"
#include <algorithm>


Game::Game()
{
	// Binding our UDP server to port
	//outSock = std::make_shared<SOCKET>();
	server.Bind(54000);
	//server.SetAcceptebleSocket(outSock);
}
void Game::Update(std::mutex& mtx)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const float dt = ft.Mark();
	
	std::lock_guard<std::mutex> lm(mtx);
	
	try
	{
		timeForSending += dt;
		// Looping our players objects
		for (auto it = clientAttr.begin(); it != clientAttr.end(); it++)
		{
			if (it->second.addr.sin_addr.S_un.S_addr)
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
					std::cout << "Right is pressed!" << std::endl;
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
		/*eraseIf(clientAttr, [](const std::pair<unsigned short, ClientAttributes>& mapy)
			{
				return mapy.second.time_since_heard_from_client > clientTimeOut;
			});*/
		
		
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

void Game::AcceptConnection(std::mutex& mtx)
{
	std::lock_guard<std::mutex> lm(mtx);
	
	
	sockaddr_in tempAddr;
	//std::cout << "Acception is working for now" << std::endl;
	server.Accept(outSock ,tempAddr);
	
	
	if (outSock != INVALID_SOCKET)
	{
		if (std::none_of(clientAttr.cbegin(), clientAttr.cend(), [this](std::pair<unsigned short, ClientAttributes> attr)
			{
				return attr.second.socket == outSock;
			}))
		{
			ULONG NonBlock = 1;
			if (ioctlsocket(outSock, FIONBIO, &NonBlock) == SOCKET_ERROR)
			{
				std::cout << "Error while accepting " << WSAGetLastError();
			}
			ClientAttributes tempAttr;
			tempAttr.socket = outSock;
			tempAttr.addr = tempAddr;
			tempAttr.state = ClientStage::Broadcast;
			tempAttr.input = {};
			tempAttr.objects = {};
			tempAttr.direction = PlayerDirection::Up;
			tempAttr.time_since_heard_from_client = 0;

			clientAttr.insert(std::pair<unsigned short, ClientAttributes>({ tempAttr.addr.sin_port }, tempAttr));

			auto it = clientAttr.find(tempAttr.addr.sin_port);
			char tempSlot[256];
			ZeroMemory(tempSlot, 256);
			server.WriteToBuffer(tempSlot, 0, it->first);
			if (server.SendingTCPMsgs(tempSlot, it->second.socket))
			{
				std::cout << "New connection will have id " << tempAttr.addr.sin_port << std::endl;
			}
			
		}
	}
	
		
	
}

void Game::UnpackingRecBuf(std::mutex& mtx)
{
	
	/*if (clientAttr.size())
	{
		std::cout << clientAttr.size() << std::endl;
	}*/
	
	sockaddr_in from ;
	int bytesReceived = server.ReceivingMsgs(recvBuffer, from);
	std::lock_guard<std::mutex> lm(mtx);

	if (bytesReceived == SOCKET_ERROR)
	{
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server.GetSocket(), SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);


		if (WSAGetLastError() == 0)
		{
			// Just passing if we catch not full message
			from = {};
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
	case ClientMessage::Input:
	{
		unsigned short slot;
		server.ReadFromBuffer(slot, recvBuffer, 1);
		//std::cout << "Client_Message::Input from \n"<< slot << std::endl;
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
	ZeroMemory(sendBuffer, 1024);
	sendBuffer[0] = (char)ServerMessage::State;
	int bytesWriten = 1;


	try
	{

		for (auto it = clientAttr.begin(); it != clientAttr.end(); it++)
		{
			if (it->second.addr.sin_addr.S_un.S_addr)
			{
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->first);

				char currState = char(it->second.state);
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, currState);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->second.objects.x);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, it->second.objects.y);

				char currDir = char(it->second.direction);
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




	//sockaddr_in to;
	//to.sin_family = AF_INET;
	//to.sin_port = htons(54000);
	//int to_length = sizeof(to);




	try
	{

		for (auto it = clientAttr.begin(); it != clientAttr.end(); it++)
		{
			if (it->second.addr.sin_addr.S_un.S_addr)
			{
				//to.sin_addr.S_un.S_addr = it->second.addr.sin_addr.S_un.S_addr;
				//to.sin_port = it->second.addr.sin_port;
				
				if (server.SendingMsgs(sendBuffer, bytesWriten, it->second.addr) != SOCKET_ERROR)
				{
					//std::cout << "Sending msg!" << std::endl;
				}
				else
				{
					std::cout << "Socket error!" << std::endl;
				}
			}

		}
		ZeroMemory(sendBuffer, 1024);
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Sending packed message error" << oor.what() << std::endl;

		//to = {};

		ZeroMemory(sendBuffer, 1024);
	}
	
	
	
}
