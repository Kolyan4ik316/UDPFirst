#include "State.h"

State::State(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Client> client)
	:
	window(window),
	client(client)
{
	if (!font.loadFromFile("arial.ttf"))
	{
		throw(std::exception("Can't find your font!"));
	}
	
}

void State::CheckForQuit()
{
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		quit = true;
	}
}

void State::FocusedState(const bool& isFocused)
{
	focused = isFocused;
}

const bool& State::GetQuit() const
{
	return quit;
}

State::~State()
{
}

const bool& State::IsFocused() const
{
	return focused;
}
