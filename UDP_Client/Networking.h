#pragma once


namespace Networking
{
	struct IP_Endpoint
	{
		unsigned int address; // converting ipAddress to integer
		unsigned short port; // taking port
	};

	enum class ClientMessage : unsigned char
	{
		Join,		// tell server we're new here
		Leave,		// tell server we're leaving
		Input 		// tell server our user input
	};

	enum class ServerMessage : unsigned char
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
}