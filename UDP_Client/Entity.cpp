#include "Entity.h"
Entity::Entity()
{
	sprite.setSize(sf::Vector2f(50.0f, 50.0f));
	sprite.setFillColor(sf::Color::White);
}
void Entity::Update(const float& dt)
{
}
void Entity::Render(sf::RenderTarget* target)
{
	target->draw(sprite);
}
void Entity::Move(const float& dt, const float& dir_x, const float& dir_y)
{
	sprite.move(sf::Vector2f(dir_x * movementSpeed * dt, dir_y * movementSpeed * dt));
}
void Entity::SetPosition(const float& pos_x, const float& pos_y)
{
	sprite.setPosition(sf::Vector2f(pos_x, pos_y));
}
Entity::~Entity()
{

}

void Entity::LoadTexture(std::string name, sf::IntRect rect)
{
	if (!texture.loadFromFile(name, rect))
	{
		std::string error = std::string("Error with loading texture! " + name);
		loadedTexture = false;
		throw(std::exception(error.c_str()));
	}
	//sprite.setTexture(texture);
	loadedTexture = true;
}
void Entity::LoadTextureFromImage(sf::Image& image, sf::IntRect rect)
{
	if (!texture.loadFromImage(image, rect))
	{
		std::string error = std::string("Error with loading texture! ");
		throw(std::exception(error.c_str()));
	}
	//sprite.setTexture(texture);
}
