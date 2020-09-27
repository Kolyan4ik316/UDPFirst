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
	return std::string(clientIP + " " + clientPort + ": " + std::string(buffer) + " bytes recived = " + std::to_string(bytesReceived));
}
void Game::Update()
{
	const float dt = ft.Mark();
	sockaddr_in from;
	bytesReceived = server.ReceivingMsgs(buffer, from);
	if (bytesReceived == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK)
		{
			std::cout << "Error with receiving data" << std::endl;
		}
	}
	//std::cout << sizeof(clientObjects[0].x);
	fromEndpoint.address = from.sin_addr.S_un.S_addr;
	fromEndpoint.port = from.sin_port;
	switch ((ClientMessage)buffer[0])
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
			buffer[0] = (char)ServerMessage::Join_Result;
			if (slot != unsigned short(-1))
			{
				buffer[1] = 1;
				//std::cout << "client will be assigned to slot \n" << slot << std::endl;
				memcpy(&buffer[2], &slot, 2);
				server.SendingMsgs(buffer, 4, from);
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
				buffer[1] = 0;
				server.SendingMsgs(buffer, 2, from);
			}
		}
		break;
		case ClientMessage::Leave:
		{
			//std::cout << "Client_Message::Leave from \n" << fromEndpoint.address << std::endl;
			unsigned short slot;
			memcpy(&slot, &buffer[1], 2);

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
			memcpy(&slot, &buffer[1], 2);
			//std::cout << slot << "\nClient_Message::Input from "<<fromEndpoint.address << std::endl;
			//slot -= 1;
			if (clientEndpoints[slot] == fromEndpoint)
			{
				char input = buffer[3];

				clientInputs[slot].up = input & 0x1;
				clientInputs[slot].down = input & 0x2;
				clientInputs[slot].left = input & 0x4;
				clientInputs[slot].right = input & 0x8;

				time_since_heard_from_clients[slot] = 0.0f;
				//std::cout << "Client_Message::Input from " << slot - 1 <<" "<< int(input);
			}
		}
		break;
	}

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
	buffer[0] = (char)ServerMessage::State;
	int bytesWriten = 1;
	for (unsigned short i = 0; i < Max_Clients; ++i)
	{
		if (clientEndpoints[i].address)
		{
			memcpy(&buffer[bytesWriten], &i, sizeof(i));
			bytesWriten += sizeof(i);

			memcpy(&buffer[bytesWriten], &clientObjects[i].x, sizeof(clientObjects[i].x));
			bytesWriten += sizeof(clientObjects[i].x);

			memcpy(&buffer[bytesWriten], &clientObjects[i].y, sizeof(clientObjects[i].y));
			bytesWriten += sizeof(clientObjects[i].y);

			memcpy(&buffer[bytesWriten], &clientObjects[i].facing, sizeof(clientObjects[i].facing));
			bytesWriten += sizeof(clientObjects[i].facing);
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
			//std::cout << buffer << std::endl;

			server.SendingMsgs(buffer, bytesWriten, to);
		}
	}
}
bool Game::IsRunning() const
{
	return isRunning;
}

void Game::UnpackingRecBuf()
{
	bytesReceived = server.ReceivingMsgs(std::ref(buffer));
}

void Game::UnpackingRecBuf(sockaddr_in& from)
{
	bytesReceived = server.ReceivingMsgs(std::ref(buffer), std::ref(from));
}

void Game::PackingSendBuf(int sizeOfBuffer)
{
	//ZeroMemory(sendBuf, 1024);

	server.SendingMsgs(std::ref(buffer), std::ref(sizeOfBuffer));
}

void Game::PackingSendBuf(int sizeOfBuffer, sockaddr_in& to)
{
	//ZeroMemory(sendBuf, 1024);

	server.SendingMsgs(std::ref(buffer), std::ref(sizeOfBuffer), std::ref(to));
}
