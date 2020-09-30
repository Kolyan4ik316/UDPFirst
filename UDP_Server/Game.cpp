#include "Game.h"



Game::Game()
{
	server.Bind(54000);
	
	for (unsigned short i = 0; i < Max_Clients; ++i)
	{
		clientEndpoints[i] = {};
	}
	
}
std::string Game::RecivedFromClient() const
{
	return std::string(clientIP + " " + clientPort + ": " + std::string(recvBuffer) + " bytes recived = " + std::to_string(bytesReceived));
}
void Game::Update()
{
	const float dt = ft.Mark();
	

	for (unsigned short i = 0; i < Max_Clients; ++i)
	{
		if (clientEndpoints[i].address)
		{
			if (clientInputs[i].up)
			{
				//std::cout << "client input up" << std::endl;
				clientObjects[i].speed += acceleration * dt;
				if (clientObjects[i].speed > maxSpeed)
				{
					clientObjects[i].speed = maxSpeed;
				}
			}
			if (clientInputs[i].down)
			{
				//std::cout << "client input down" << std::endl;
				clientObjects[i].speed -= acceleration * dt;
				if (clientObjects[i].speed < 0.0f)
				{
					clientObjects[i].speed = 0.0f;
				}
			}
			if (clientInputs[i].left)
			{
				//std::cout << "client input left" << std::endl;
				clientObjects[i].facing -= turnSpeed * dt;
			}
			if (clientInputs[i].right)
			{
				//std::cout << "client input right" << std::endl;
				clientObjects[i].facing += turnSpeed * dt;
			}

			clientObjects[i].x += clientObjects[i].speed * dt * sinf(clientObjects[i].facing);
			clientObjects[i].y += clientObjects[i].speed * dt * cosf(clientObjects[i].facing);

			time_since_heard_from_clients[i] += dt;
			if (time_since_heard_from_clients[i] > clientTimeOut)
			{
				clientEndpoints[i] = {};
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
	bytesReceived = server.ReceivingMsgs(recvBuffer, from);
	if (bytesReceived == SOCKET_ERROR)
	{
		std::cout << "Error with receiving data" << std::endl;
	}
	fromEndpoint.address = from.sin_addr.S_un.S_addr;
	fromEndpoint.port = from.sin_port;
	switch ((ClientMessage)recvBuffer[0])
	{

	case ClientMessage::Join:
	{
		//std::cout << "Client_Message::Join from \n" << fromEndpoint.address << std::endl;
		unsigned short slot = unsigned short(-1);
		for (unsigned short i = 0; i < Max_Clients; ++i)
		{
			if (clientEndpoints[i].address == 0)
			{
				slot = i;

				break;
			}
		}
		sendBuffer[0] = (char)ServerMessage::Join_Result;
		if (slot != unsigned short(-1))
		{
			sendBuffer[1] = 1;
			//std::cout << "client will be assigned to slot \n" << slot << std::endl;
			//memcpy(&sendBuffer[2], &slot, 2);
			server.WriteToBuffer(sendBuffer, 2, slot);
			server.SendingMsgs(sendBuffer, 4, from);
			if (server.IsSended())
			{
				clientEndpoints[slot] = fromEndpoint;
				time_since_heard_from_clients[slot] = 0.0f;
				clientObjects[slot] = {};
				clientInputs[slot] = {};
			}
		}
		else
		{
			sendBuffer[1] = 0;
			server.SendingMsgs(sendBuffer, 2, from);
		}
	}
	break;
	case ClientMessage::Leave:
	{
		//std::cout << "Client_Message::Leave from \n" << fromEndpoint.address << std::endl;
		unsigned short slot;
		//memcpy(&slot, &recvBuffer[1], 2);
		server.ReadFromBuffer(slot, recvBuffer, 1);

		if (clientEndpoints[slot] == fromEndpoint)
		{
			clientEndpoints[slot] = {};
		}
	}
	break;
	case ClientMessage::Input:
	{
		//std::cout << "Client_Message::Input from \n"<<fromEndpoint.address << std::endl;
		unsigned short slot;

		//memcpy(&slot, &recvBuffer[1], 2);
		server.ReadFromBuffer(slot, recvBuffer, 1);
		//std::cout << slot << "\nClient_Message::Input from "<<fromEndpoint.address << std::endl;
		if (clientEndpoints[slot] == fromEndpoint)
		{
			char input = recvBuffer[3];

			clientInputs[slot].up = input & 0x1;
			clientInputs[slot].down = input & 0x2;
			clientInputs[slot].left = input & 0x4;
			clientInputs[slot].right = input & 0x8;

			time_since_heard_from_clients[slot] = 0.0f;
		}
	}
	break;
	}
}

void Game::PackingSendBuf()
{
	sendBuffer[0] = (char)ServerMessage::State;
	int bytesWriten = 1;
	for (unsigned short i = 0; i < Max_Clients; ++i)
	{
		if (clientEndpoints[i].address)
		{
			bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, i);

			bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects[i].x);

			bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects[i].y);

			bytesWriten += server.WriteToBuffer(sendBuffer, bytesWriten, clientObjects[i].facing);
		}
	}

	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(54000);
	int to_length = sizeof(to);

	for (unsigned char i = 0; i < Max_Clients; ++i)
	{
		if (clientEndpoints[i].address)
		{
			to.sin_addr.S_un.S_addr = clientEndpoints[i].address;
			to.sin_port = clientEndpoints[i].port;
			//std::cout << sendBuffer << std::endl;

			server.SendingMsgs(sendBuffer, bytesWriten, to);
		}
	}
}
