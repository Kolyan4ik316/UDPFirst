#pragma once
#include "Vec2.h"


namespace Networking
{
	struct IP_Endpoint
	{
		unsigned int address; // converting ipAddress to integer
		unsigned short port; // taking port
	};

	enum class ClientMessage : char
	{
		Join,		// tell server we're new here
		Leave,		// tell server we're leaving
		Input, 		// tell server our user input
		Empty		// tell server our user nothing to do
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
	};
}