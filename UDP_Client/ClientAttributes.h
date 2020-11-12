#pragma once
#include "Networking.h"
#include <SFML/Graphics.hpp>
using namespace Networking;

class ClientAttributes
{
public:
	ClientAttributes() = default;
	ClientAttributes(unsigned short& slot, const ClientStage& state, PlayerState& objects, float& time)
		:
		slot(slot),
		stage(state),
		objects(objects),
		time_since_heard_from_client(time)
	{}
	void SetGraphicsAttr(const sf::Vector2f size = sf::Vector2f(50.0f, 50.0f), const sf::Color color = sf::Color::Red)
	{
		shape.setSize(size);
		shape.setFillColor(color);
	}
	unsigned short slot;
	ClientStage stage;
	PlayerState objects;
	float time_since_heard_from_client;
	sf::RectangleShape shape;
	


};