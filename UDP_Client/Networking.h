#pragma once


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
	};

	struct PlayerState
	{
		float x, y;
	};

	template<class containerT, class predicateT>
	void eraseIf(containerT& items, const predicateT& predicate)
	{
		for (auto it = items.begin(); it != items.end(); )
		{
			if (predicate(*it))
			{
				std::swap(*it, items.back());
				items.pop_back();
				//it = items.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}