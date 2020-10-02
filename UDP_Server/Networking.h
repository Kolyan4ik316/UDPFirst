#pragma once
#include "Vec2.h"

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
		Leave,		// tell server we're leaving
		Input 		// tell server our user input
	};

	enum class ServerMessage : char
	{
		Join_Result,// tell client they're accepted/rejected
		State 		// tell client game state
	};

	struct PlayerInput
	{
		int up; 
		int down; 
		int left; 
		int right;
	};

	struct PlayerState
	{
		float x, y;
		float facing;
		float speed;
	};
}