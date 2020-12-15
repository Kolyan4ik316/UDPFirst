#pragma once

#include <WS2tcpip.h>

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

#pragma comment (lib, "ws2_32.lib")

namespace Networking
{
	struct IP_Endpoint
	{
		IP_Endpoint() = default;
		IP_Endpoint(unsigned int address, unsigned short port)
			:
			address(address),
			port(port)
		{}
		unsigned int address; // converting ipAddress to integer
		unsigned short port; // taking port

		bool operator == (const IP_Endpoint& rhs) const
		{
			return this->address == rhs.address && this->port == rhs.port;
		}
	};
	
	
	

	enum class ClientMessage : char
	{
		Join,		// tell server we're new here
		//Leave,		// tell server we're leaving
		Input 		// tell server our user input
	};

	enum class ServerMessage : char
	{
		Join_Result,// tell client they're accepted/rejected
		State 		// tell client game state
	};

	enum class ClientStage : char
	{
		Broadcast,	// Main channel (showing rooms)
		Room,		// In Room
		Game,		// In Game itself
		Leave		// Leaving from server
	};

	struct PlayerInput
	{
		int up; 
		int down; 
		int left; 
		int right;
		int empty;
	};

	enum class PlayerDirection : char
	{
		Up,
		Down,
		Left,
		Right,
	};

	struct PlayerState
	{
		float x, y;
	};
	struct ClientAttributes
	{
		SOCKET socket;
		sockaddr_in addr;
		ClientStage state;
		PlayerState objects;
		PlayerInput input;
		PlayerDirection direction;
		float time_since_heard_from_client;
	};
	template<class containerT, class predicateT>
	void eraseIf(containerT& items, const predicateT& predicate)
	{
		for (auto it = items.begin(); it != items.end(); )
		{
			if (predicate(*it))
			{
				it = items.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}