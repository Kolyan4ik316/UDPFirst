#include "Game.h"
#include <algorithm>
#include <memory>


#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)


Game::Game()
{
	server.Bind(9999);
	

	
}
std::string Game::RecivedFromClient() const
{
	return std::string(clientIP + " " + clientPort + ": " + std::string(recvBuffer) + " bytes recived = ");
}
void Game::Update()
{
	const float dt = ft.Mark();
	

	for (unsigned short i = 0; i < clientEndpoints.size(); ++i)
	{
		if (clientEndpoints.at(i).address)
		{
			if (clientInputs.at(i).up)
			{
				//std::cout << "client input up" << std::endl;
				clientObjects.at(i).speed += acceleration * dt;
				if (clientObjects.at(i).speed > maxSpeed)
				{
					clientObjects.at(i).speed = maxSpeed;
				}
			}
			if (clientInputs.at(i).down)
			{
				//std::cout << "client input down" << std::endl;
				clientObjects.at(i).speed -= acceleration * dt;
				if (clientObjects.at(i).speed < 0.0f)
				{
					clientObjects.at(i).speed = 0.0f;
				}
			}
			if (clientInputs.at(i).left)
			{
				//std::cout << "client input left" << std::endl;
				clientObjects.at(i).facing -= turnSpeed * dt;
			}
			if (clientInputs.at(i).right)
			{
				//std::cout << "client input right" << std::endl;
				clientObjects.at(i).facing += turnSpeed * dt;
			}

			clientObjects.at(i).x += clientObjects.at(i).speed * dt * sinf(clientObjects.at(i).facing);
			clientObjects.at(i).y += clientObjects.at(i).speed * dt * cosf(clientObjects.at(i).facing);

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
bool Game::IsRunning() const
{
	return isRunning;
}

void Game::UnpackingRecBuf()
{

	sockaddr_in from;
	IP_Endpoint fromEndpoint;
	int bytesReceived;
	bytesReceived = server.ReceivingMsgs(recvBuffer, from);

	if (bytesReceived == SOCKET_ERROR)
	{

		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server.GetSocket(), SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

		std::cout << "Error with receiving data " << WSAGetLastError() << std::endl;

	}
	fromEndpoint.address = from.sin_addr.S_un.S_addr;
	fromEndpoint.port = from.sin_port;
	switch ((ClientMessage)recvBuffer[0])
	{

	case ClientMessage::Join:
	{
		unsigned short slot = unsigned short(-1);
		//std::cout << "Client_Message::Join from \n" << fromEndpoint.address << std::endl;
		
		clientEndpoints.push_back({});
		time_since_heard_from_clients.push_back(0.0f);
		clientObjects.push_back({});
		clientInputs.push_back({});

		for (unsigned short i = 0; i < clientEndpoints.size(); ++i)
		{
			if (clientEndpoints.at(i).address == 0)
			{
				slot = i;
				
				
				//clientEndpoints.resize(slot + 1);
				break;
			}
		}
		

		sendBuffer[0] = (char)ServerMessage::Join_Result;
		if (slot != unsigned short(-1))
		{
			sendBuffer[1] = 1;
			//std::cout << "client will be assigned to slot \n" << slot << std::endl;
			server.WriteToBuffer(sendBuffer, 2, slot);
			server.SendingMsgs(sendBuffer, 4, from);
			if (server.IsSended())
			{
				
				clientEndpoints.at(slot) = fromEndpoint;
				time_since_heard_from_clients.at(slot) = 0.0f;
				clientObjects.at(slot) = {};
				clientInputs.at(slot) = {};
			}
		}
		else
		{
			sendBuffer[1] = 0;
			server.SendingMsgs(sendBuffer, 2, from);

			std::cout << "Can't let client in " << fromEndpoint.address << ": " << fromEndpoint.port << std::endl;
			std::swap(clientEndpoints.at(slot), clientEndpoints.back());
			clientEndpoints.pop_back();

			std::swap(time_since_heard_from_clients.at(slot), time_since_heard_from_clients.back());
			time_since_heard_from_clients.pop_back();

			std::swap(clientObjects.at(slot), clientObjects.back());
			clientObjects.pop_back();

			std::swap(clientInputs.at(slot), clientInputs.back());
			clientInputs.pop_back();

		}
	}
	break;
	case ClientMessage::Leave:
	{
		std::cout << "Client_Message::Leave from " << fromEndpoint.address <<": "<<fromEndpoint.port << std::endl;
		unsigned short slot;
		server.ReadFromBuffer(slot, recvBuffer, 1);
		try
		{
			if (clientEndpoints.at(slot) == fromEndpoint)
			{

				std::swap(clientEndpoints.at(slot), clientEndpoints.back());
				clientEndpoints.pop_back();
				
				std::swap(time_since_heard_from_clients.at(slot), time_since_heard_from_clients.back());
				time_since_heard_from_clients.pop_back();
				
				std::swap(clientObjects.at(slot), clientObjects.back());
				clientObjects.pop_back();
				
				std::swap(clientInputs.at(slot), clientInputs.back());
				clientInputs.pop_back();
			}
		}
		catch (const std::out_of_range& oor)
		{
			break;
		}
	}
	break;
	case ClientMessage::Input:
	{
		//std::cout << "Client_Message::Input from \n"<<fromEndpoint.address << std::endl;
		unsigned short slot;

		//memcpy(&slot, &recvBuffer[1], 2);
		server.ReadFromBuffer(slot, recvBuffer, 1);
		
		try
		{
			if (clientEndpoints.at(slot) == fromEndpoint)
			{
				char input = recvBuffer[3];

				clientInputs.at(slot).up = input & 0x1;
				clientInputs.at(slot).down = input & 0x2;
				clientInputs.at(slot).left = input & 0x4;
				clientInputs.at(slot).right = input & 0x8;

				time_since_heard_from_clients.at(slot) = 0.0f;
			}
		}
		catch (const std::out_of_range& oor)
		{
			break;
		}
		
		
	}
	break;
	}
}

void Game::PackingSendBuf()
{
	sendBuffer[0] = (char)ServerMessage::State;
	int bytesWriten = 1;
	for (unsigned short i = 0; i < clientEndpoints.size(); ++i)
	{
		try
		{
			if (clientEndpoints.at(i).address)
			{
				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, i);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects.at(i).x);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects.at(i).y);

				bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects.at(i).facing);
			}
		}
		catch (...)
		{
		}
		
	}

	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(9999);
	int to_length = sizeof(to);
	
	
	
	for (unsigned char i = 0; i < clientEndpoints.size(); ++i)
	{
		try
		{
			if (clientEndpoints.at(i).address)
			{
				to.sin_addr.S_un.S_addr = clientEndpoints.at(i).address;
				to.sin_port = clientEndpoints.at(i).port;
				//std::cout << sendBuffer << std::endl;

				server.SendingMsgs(sendBuffer, bytesWriten, to);
			}
		}
		catch (...)
		{

		}
	}
}
