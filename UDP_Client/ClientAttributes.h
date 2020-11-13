#pragma once
#include "Networking.h"
#include <SFML/Graphics.hpp>
using namespace Networking;

class ClientAttributes
{
public:
	ClientAttributes() = default;
	ClientAttributes(unsigned short& slot, const ClientStage& state, PlayerState& objects, PlayerDirection& direction, float& time)
		:
		slot(slot),
		stage(state),
		objects(objects),
		time_since_heard_from_client(time)
	{}
	void Update(const PlayerDirection& direction)
	{
		if (direction == PlayerDirection::Up)
		{
			sprite.setRotation(360);
		}
		else if (direction == PlayerDirection::Down)
		{
			sprite.setRotation(180);
		}
		else if (direction == PlayerDirection::Left)
		{
			sprite.setRotation(270);
		}
		else if (direction == PlayerDirection::Right)
		{
			sprite.setRotation(90);
		}
	}
	unsigned short slot;
	ClientStage stage;
	PlayerState objects;
	float time_since_heard_from_client;
	sf::Text text;
	sf::Sprite sprite;
	PlayerDirection direction;
	
	


};