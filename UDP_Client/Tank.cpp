#include "Tank.h"
Tank::Tank()
{
	//LoadTexture("battle_city_sprites.png", sf::IntRect(0, 0, 16, 16));
}
Tank::Tank(const unsigned short& slot)
	:
	slot(slot)
{
	Tank();
	
}
void Tank::Update(const float& dt)
{
	sprite.setOrigin(8, 8);
	/*if (loadedTexture)
	{
		sprite.scale(2.7f, 2.7f);
		loadedTexture = false;
	}*/
	text.setString(std::to_string(slot));
	text.setPosition(sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y - 25.0f));
	time_since_heard_from_client += dt;
}
void Tank::Render(sf::RenderTarget* target)
{
	target->draw(sprite);
	target->draw(text);
}
void Tank::SetDirection(const Networking::PlayerDirection& direction)
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
const bool Tank::TimeOutReached() const
{
	return time_since_heard_from_client > 15.0f;
}
const bool Tank::LeaveCondition() const
{
	return TimeOutReached() || stage == ClientStage::Leave;
}
Tank::~Tank()
{

}