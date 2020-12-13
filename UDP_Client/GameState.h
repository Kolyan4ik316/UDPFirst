#pragma once
#include "State.h"
#include "Entity.h"
#include "Tank.h"

class GameState : public State
{
public:
	GameState(std::shared_ptr<sf::RenderWindow> window);
	void Update(const float& dt) override;
	void Render(sf::RenderTarget* target = nullptr) override;
	void UpdateInput(const float& dt) override;
	void UnpackMsg(Client& client, std::mutex& mtx) override;
	void PackMsg(Client& client, std::mutex& mtx) override;
	void EndState() override;
	virtual ~GameState();
private:
	Tank player;
	std::vector<Tank> otherPlayers;
	char input = 0;
};